#!/bin/sh
# patch-php-mk.sh DATEI - fuegt PHP 7.4 wieder in den aktuellen Upstream-Mk/Uses/php.mk ein.
# Jede Aenderung wird geprueft; passt ein Anker nicht mehr -> Abbruch mit Meldung (Upstream hat umgebaut).
set -eu
f=${1:?Aufruf: patch-php-mk.sh Mk/Uses/php.mk}
die() { echo "FEHLER php.mk-Anpassung: $1 (Upstream php.mk umgebaut? Skript anpassen)" >&2; exit 1; }
[ -f "$f" ] || die "$f fehlt"
grep -q '[$][{]PHP_VER[}] == 74' "$f" && { echo "php.mk: 74 schon enthalten"; exit 0; }
t=$(mktemp)

# 1. 74 zu den erlaubten Versionen (erzeugt auch das php74-Flavor)
sed -E 's/^(_ALL_PHP_VERSIONS=[[:space:]]*)/\174 /' "$f" > "$t"
grep -q -E '^_ALL_PHP_VERSIONS=[[:space:]]*74 ' "$t" || die "_ALL_PHP_VERSIONS"
cat "$t" > "$f"

# 2. PHP_EXT_DIR/PHP_EXT_INC fuer 7.4 (als erster Zweig der if/elif-Kette)
awk '
  prev ~ /bsd[.]default-versions[.]mk in sync/ && !done && $0 ~ /^[.][[:space:]]+if [$][{]PHP_VER[}] == [0-9]+[[:space:]]*$/ {
      ind = $0; sub(/if.*/, "", ind)
      print ind "if ${PHP_VER} == 74"
      print "PHP_EXT_DIR=   20190902"
      print "PHP_EXT_INC=    hash pcre spl"
      sub(/if/, "elif"); done = 1
  }
  { print; prev = $0 }' "$f" > "$t"
grep -q '^PHP_EXT_DIR=   20190902' "$t" && grep -q -E '^[.][[:space:]]+elif [$][{]PHP_VER[}] == [0-9]+' "$t" || die "PHP_EXT_DIR-Block"
cat "$t" > "$f"

# 3. erlaubte USE_PHP-Erweiterungen fuer 7.4 (random gibt es erst ab 8.2)
awk '
  /^_USE_PHP_VER[0-9]+=/ && !done { print "_USE_PHP_VER74=\t${_USE_PHP_ALL:Nrandom} bitset imagick opcache redis"; done = 1 }
  { print }' "$f" > "$t"
grep -q '^_USE_PHP_VER74=' "$t" || die "_USE_PHP_VER"
cat "$t" > "$f"

# 4. Erweiterungs-Abhaengigkeiten, die es in 7.4 als eigene Ports gibt
awk '
  /^[.][[:space:]]+for extension in [$][{]USE_PHP[}]/ && !done {
      print ".    if ${PHP_VER} == 74"
      print "json_DEPENDS=\tdevel/php${PHP_VER}-json"
      print "openssl_DEPENDS=security/php${PHP_VER}-openssl"
      print "xmlrpc_DEPENDS=\tnet/php${PHP_VER}-xmlrpc"
      print ".    endif"
      print ""
      done = 1
  }
  { print }' "$f" > "$t"
grep -q '^json_DEPENDS=' "$t" || die "for extension-Schleife"
cat "$t" > "$f"
rm -f "$t"

# 5. Struktur-Check: gleich viele .if/.for wie .endif/.endfor
awk '/^[.][[:space:]]*(if|ifdef|ifndef|for)[[:space:]]/ {o++} /^[.][[:space:]]*(endif|endfor)/ {c++}
     END { if (o != c) { print "if/endif unausgeglichen: " o " vs " c > "/dev/stderr"; exit 1 } }' "$f" || die "Struktur"
echo "php.mk: PHP 7.4 eingefuegt"
