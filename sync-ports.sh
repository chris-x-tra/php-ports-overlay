#!/bin/sh
# sync-ports.sh - baut TARGET (= /usr/ports.php74) aus Upstream-FreeBSD-Ports + eigenem Overlay (PHP 7.4)
# sync-ports.sh - baut TARGET (= /usr/ports.php56) aus Upstream-FreeBSD-Ports + eigenem Overlay (PHP 5.6)
#
# Regel: In TARGET wird NIE von Hand editiert. Aenderungen gehen ins Overlay-Repo,
# TARGET wird bei jedem Lauf neu aus Upstream + Overlay erzeugt.
#
# Overlay-Repo (Layout wie ein Ports-Tree):
#   lang/php74/ ...   archivers/php74-zlib/ ...   (alle php74-Ports)
#   patch-php-mk.sh   fuegt PHP 7.4 in den jeweils AKTUELLEN Upstream-Mk/Uses/php.mk ein
#   profiles/NAME/    (optional) Ports nur fuer ein Profil, z.B. profiles/php56/databases/pecl-memcache
#                     (alte 5.6-Version, die eine neuere Upstream-Version ueberschreibt).
#                     Aktivierung: PROFILE=php56 sync-ports.sh
#   OVERRIDES         (optional) Upstream-Dateien, die das Overlay als ganze Kopie ueberschreibt
#   UPSTREAM_BASE     Upstream-Commit, auf dem diese Kopien beruhen (Drift-Warnung)
#
# Benoetigt: pkg install git rsync
#
# --> Ablauf:
# cd /usr/ports/
# git pull
# cd /usr/local/src/php-ports-overlay
# Aufruf:  sync-ports.sh            # normaler Lauf
#          PROFILE=php56 TARGET=/usr/jails/php56jail/usr/ports.php56/ sync-ports.sh   # (Tree fuer das php5-Jail)
#          PROFILE=php74 TARGET=/usr/jails/php74jail/usr/ports.php74/ sync-ports.sh   # (Tree fuer das php7-Jail)
#          sync-ports.sh --accept   # danach UPSTREAM_BASE auf aktuellen Upstream-Stand setzen

set -eu

UPSTREAM=${UPSTREAM:-/usr/ports}
OVERLAY=${OVERLAY:-/usr/local/src/php-ports-overlay/}
TARGET=${TARGET:-/usr/jails/php56jail/usr/ports.php56/
#TARGET=${TARGET:-/usr/jails/php74jail/usr/ports.php74/
PROFILE=${PROFILE:-}           # leer = nur gemeinsamer Teil; php56 = zusaetzlich profiles/php56
BRANCH=${BRANCH:-main}         # oder z.B. 2026Q4 (Quartalszweig, weniger Aenderungen)

[ -d "$UPSTREAM/.git" ] || { echo "FEHLER: $UPSTREAM ist kein Git-Klon" >&2; exit 1; }
[ -d "$OVERLAY/.git" ]  || { echo "FEHLER: $OVERLAY ist kein Git-Klon" >&2; exit 1; }

git -C "$UPSTREAM" fetch -q origin
git -C "$UPSTREAM" checkout -q "$BRANCH"
git -C "$UPSTREAM" merge -q --ff-only "origin/$BRANCH"
if git -C "$OVERLAY" remote | grep -q .; then
    git -C "$OVERLAY" pull -q --ff-only
fi

# Drift-Check: hat Upstream Dateien geaendert, die wir ueberschreiben?
BASE=$(cat "$OVERLAY/UPSTREAM_BASE" 2>/dev/null || true)
if [ -n "$BASE" ] && [ -f "$OVERLAY/OVERRIDES" ]; then
    while read -r f; do
        [ -z "$f" ] && continue
        if ! git -C "$UPSTREAM" diff --quiet "$BASE" HEAD -- "$f"; then
            echo "WARNUNG: Upstream hat $f seit $BASE geaendert. Overlay-Kopie pruefen:"
            git -C "$UPSTREAM" diff --stat "$BASE" HEAD -- "$f"
        fi
    done < "$OVERLAY/OVERRIDES"
fi

# 1. Upstream spiegeln, 2. Overlay drueberlegen
rsync -a --delete --exclude .git "$UPSTREAM/" "$TARGET/"
rsync -a --exclude .git --exclude README.md --exclude sync-ports.sh \
         --exclude UPSTREAM_BASE --exclude OVERRIDES --exclude patch-php-mk.sh --exclude profiles "$OVERLAY/" "$TARGET/"
if [ -n "$PROFILE" ]; then
    [ -d "$OVERLAY/profiles/$PROFILE" ] || { echo "FEHLER: Profil $PROFILE fehlt im Overlay" >&2; exit 1; }
    rsync -a --exclude .git "$OVERLAY/profiles/$PROFILE/" "$TARGET/"
fi

# Nacharbeit: Das haben die Sunset-Commits (php56: ba09f7ee95ce, php74: 0d310d700aa0)
# in Upstream-Dateien geaendert, die sich taeglich aendern (deshalb gefiltert statt als
# Kopie im Overlay). SUNSETS: Portname-Teil:Datum der MOVED-Zeilen
SUNSETS=${SUNSETS:-"php56:2019-01-01 php74:2022-12-25"}
#  a) MOVED: die Entfernungs-/Umzugszeilen dieser Ports streichen, sonst wuerden
#     portmaster/portupgrade installierte Pakete "umziehen" bzw. als entfernt melden.
tmp=$(mktemp)
for sd in $SUNSETS; do
    name=${sd%%:*}; date=${sd##*:}
    grep -v -E "^[^|]*${name}[^|]*[|][^|]*[|]${date}[|]" "$TARGET/MOVED" > "$tmp" || true
    cat "$tmp" > "$TARGET/MOVED"
done
rm -f "$tmp"
#  b) Kategorie-Makefiles: SUBDIR-Eintraege aller Overlay-Ports (gemeinsam + Profil)
#     einfuegen, falls Upstream sie nicht (mehr) hat. Noetig fuer make index/search;
#     zum Bauen einzelner Ports nicht.
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
[ -n "$PROFILE" ] && add_subdirs "$OVERLAY/profiles/$PROFILE"
#  c) Mk/Uses/php.mk: PHP 7.4 wieder zulassen (bricht ab, wenn Upstream umgebaut hat)
sh "$OVERLAY/patch-php-mk.sh" "$TARGET/Mk/Uses/php.mk"

echo "OK: $TARGET = $(git -C "$UPSTREAM" rev-parse --short HEAD) ($BRANCH) + Overlay $(git -C "$OVERLAY" rev-parse --short HEAD)"
for v in 74 56; do [ -d "$TARGET/lang/php$v" ] && echo "php$v Version: $(make -C "$TARGET/lang/php$v" -V PORTVERSION 2>/dev/null || echo 'make -V fehlgeschlagen')"; done

if [ "${1:-}" = "--accept" ]; then
    git -C "$UPSTREAM" rev-parse HEAD > "$OVERLAY/UPSTREAM_BASE"
    echo "UPSTREAM_BASE aktualisiert, im Overlay-Repo committen."
fi

