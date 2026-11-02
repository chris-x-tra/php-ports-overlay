#!/bin/sh
# sync-ports.sh - builds a ports tree (TARGET) for a legacy PHP jail from the current
#                 upstream FreeBSD ports tree plus our own overlay repository.
#                 Used for:  php74jail -> TARGET .../ports.php74   (no profile needed)
#                            php5jail  -> TARGET .../ports.php56   (PROFILE=php56)
#
# Rule: NEVER edit TARGET by hand. Changes go into the overlay repo; TARGET is
# regenerated from upstream + overlay on every run.
#
# Overlay repo layout (same as a ports tree):
#   lang/php74/ ...   archivers/php74-zlib/ ...   (all php74 / php56 ports)
#   patch-php-mk.sh   inserts PHP 7.4 / 5.6 into the CURRENT upstream Mk/Uses/php.mk
#   profiles/NAME/    (optional) ports for one profile only, e.g.
#                     profiles/php56/databases/pecl-memcache (old 5.6-era version that
#                     replaces a newer upstream version). Enable with PROFILE=NAME.
#   OVERRIDES         (optional) upstream files that the overlay replaces as a whole copy
#   UPSTREAM_BASE     upstream commit the overlay copies are based on (drift warning)
#
# Requires: pkg install git rsync
#
# Usage (TARGET is mandatory; there is deliberately no default, to avoid syncing
# into the wrong jail):
#   cd /usr/local/src/php-ports-overlay
#   PROFILE=php56 TARGET=/usr/jails/php56jail/usr/ports.php56/ ./sync-ports.sh   # tree for the php5 jail
#   TARGET=/usr/jails/php74jail/usr/ports.php74/ ./sync-ports.sh                 # tree for the php7 jail
#   ./sync-ports.sh --accept   # (with TARGET) afterwards: record current upstream commit as UPSTREAM_BASE
#
# Environment: UPSTREAM (default /usr/ports), OVERLAY, TARGET, PROFILE, BRANCH, SUNSETS

set -eu

UPSTREAM=${UPSTREAM:-/usr/ports}
OVERLAY=${OVERLAY:-/usr/local/src/php-ports-overlay}
TARGET=${TARGET:-}
PROFILE=${PROFILE:-}           # empty = common part only; php56 = additionally profiles/php56
BRANCH=${BRANCH:-main}         # or e.g. 2026Q4 (quarterly branch, fewer changes)

[ -n "$TARGET" ] || { echo "ERROR: TARGET is not set (e.g. TARGET=/usr/jails/php74jail/usr/ports.php74/)" >&2; exit 1; }
[ -d "$UPSTREAM/.git" ] || { echo "ERROR: $UPSTREAM is not a git clone" >&2; exit 1; }
[ -d "$OVERLAY/.git" ]  || { echo "ERROR: $OVERLAY is not a git clone" >&2; exit 1; }
[ "$TARGET" != "$UPSTREAM" ] || { echo "ERROR: TARGET must not be the upstream tree" >&2; exit 1; }
mkdir -p "$TARGET"

git -C "$UPSTREAM" fetch -q origin
git -C "$UPSTREAM" checkout -q "$BRANCH"
git -C "$UPSTREAM" merge -q --ff-only "origin/$BRANCH"
if git -C "$OVERLAY" remote | grep -q .; then
    git -C "$OVERLAY" pull -q --ff-only
fi

# Drift check: did upstream change files that we override with a full copy?
BASE=$(cat "$OVERLAY/UPSTREAM_BASE" 2>/dev/null || true)
if [ -n "$BASE" ] && [ -f "$OVERLAY/OVERRIDES" ]; then
    while read -r f; do
        [ -z "$f" ] && continue
        if ! git -C "$UPSTREAM" diff --quiet "$BASE" HEAD -- "$f"; then
            echo "WARNING: upstream changed $f since $BASE. Check the overlay copy:"
            git -C "$UPSTREAM" diff --stat "$BASE" HEAD -- "$f"
        fi
    done < "$OVERLAY/OVERRIDES"
fi

# 1. mirror upstream, 2. lay the overlay on top
rsync -a --delete --exclude .git "$UPSTREAM/" "$TARGET/"
rsync -a --exclude .git --exclude README.md --exclude README.txt --exclude sync-ports.sh \
         --exclude UPSTREAM_BASE --exclude OVERRIDES --exclude patch-php-mk.sh --exclude profiles "$OVERLAY/" "$TARGET/"
if [ -n "$PROFILE" ]; then
    if [ -d "$OVERLAY/profiles/$PROFILE" ]; then
        rsync -a --exclude .git "$OVERLAY/profiles/$PROFILE/" "$TARGET/"
    else
        echo "NOTE: no profiles/$PROFILE in the overlay; using the common part only."
    fi
fi

# Post-processing: what the sunset commits (php56: ba09f7ee95ce, php74: 0d310d700aa0)
# changed in upstream files that change daily (hence filtered here instead of kept as a
# copy in the overlay). SUNSETS: port-name part:date of the MOVED lines.
SUNSETS=${SUNSETS:-"php56:2019-01-01 php74:2022-12-25"}
#  a) MOVED: drop the removal/move lines of these ports, otherwise
#     portmaster/portupgrade would "move" installed packages or report them as removed.
tmp=$(mktemp)
for sd in $SUNSETS; do
    name=${sd%%:*}; date=${sd##*:}
    grep -v -E "^[^|]*${name}[^|]*[|][^|]*[|]${date}[|]" "$TARGET/MOVED" > "$tmp" || true
    cat "$tmp" > "$TARGET/MOVED"
done
rm -f "$tmp"
#  b) Category Makefiles: add SUBDIR entries for all overlay ports (common + profile)
#     if upstream no longer has them. Needed for make index/search; not needed to
#     build individual ports.
add_subdirs() {
    for d in "$1"/*/*/; do
        [ -f "${d}Makefile" ] || continue
        port=$(basename "$d"); cat=$(basename "$(dirname "$d")")
        mk="$TARGET/$cat/Makefile"
        [ -f "$mk" ] && grep -q 'SUBDIR' "$mk" || continue
        grep -q -E "^[[:space:]]*SUBDIR \+= $port\$" "$mk" && continue
        awk -v line="    SUBDIR += $port" '
            /bsd\.port\.subdir\.mk/ && !done { print line; done = 1 }
            { print }' "$mk" > "$mk.new" && mv "$mk.new" "$mk"
    done
}
add_subdirs "$OVERLAY"
if [ -n "$PROFILE" ] && [ -d "$OVERLAY/profiles/$PROFILE" ]; then
    add_subdirs "$OVERLAY/profiles/$PROFILE"
fi
#  c) Mk/Uses/php.mk: re-enable PHP 7.4 / 5.6 (aborts if upstream restructured the file)
sh "$OVERLAY/patch-php-mk.sh" "$TARGET/Mk/Uses/php.mk"

echo "OK: $TARGET = $(git -C "$UPSTREAM" rev-parse --short HEAD) ($BRANCH) + overlay $(git -C "$OVERLAY" rev-parse --short HEAD)"
for v in 74 56; do
    if [ -d "$TARGET/lang/php$v" ]; then
        echo "php$v version: $(make -C "$TARGET/lang/php$v" -V PORTVERSION 2>/dev/null || echo 'make -V failed')"
    fi
done

if [ "${1:-}" = "--accept" ]; then
    git -C "$UPSTREAM" rev-parse HEAD > "$OVERLAY/UPSTREAM_BASE"
    echo "UPSTREAM_BASE updated, commit it in the overlay repo."
fi
