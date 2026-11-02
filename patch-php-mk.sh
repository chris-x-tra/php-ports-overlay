#!/bin/sh
# patch-php-mk.sh FILE - re-inserts PHP 5.6 and 7.4 into the current upstream Mk/Uses/php.mk.
# Every change is verified; if an anchor no longer matches, abort with a message (upstream restructured the file).
set -eu
f=${1:?usage: patch-php-mk.sh Mk/Uses/php.mk}
die() { echo "ERROR php.mk patch: $1 (upstream php.mk restructured? adapt this script)" >&2; exit 1; }
[ -f "$f" ] || die "$f is missing"
grep -q '[$][{]PHP_VER[}] == 74' "$f" && grep -q '[$][{]PHP_VER[}] == 56' "$f" && { echo "php.mk: 56 and 74 already present"; exit 0; }
t=$(mktemp)

# 1. add 56 and 74 to the allowed versions (this also creates the flavors php56/php74)
sed -E 's/^(_ALL_PHP_VERSIONS=[[:space:]]*)/\156 74 /' "$f" > "$t"
grep -q -E '^_ALL_PHP_VERSIONS=[[:space:]]*56 74 ' "$t" || die "_ALL_PHP_VERSIONS"
cat "$t" > "$f"

# 2. PHP_EXT_DIR/PHP_EXT_INC for 7.4 and 5.6 (first branches of the if/elif chain)
awk '
  prev ~ /bsd[.]default-versions[.]mk in sync/ && !done && $0 ~ /^[.][[:space:]]+if [$][{]PHP_VER[}] == [0-9]+[[:space:]]*$/ {
      ind = $0; sub(/if.*/, "", ind)
      print ind "if ${PHP_VER} == 74"
      print "PHP_EXT_DIR=   20190902"
      print "PHP_EXT_INC=    hash pcre spl"
      print ind "elif ${PHP_VER} == 56"
      print "PHP_EXT_DIR=   20131226"
      print "PHP_EXT_INC=    pcre spl"
      sub(/if/, "elif"); done = 1
  }
  { print; prev = $0 }' "$f" > "$t"
grep -q '^PHP_EXT_DIR=   20190902' "$t" && grep -q '^PHP_EXT_DIR=   20131226' "$t" && grep -q -E '^[.][[:space:]]+elif [$][{]PHP_VER[}] == [0-9]+' "$t" || die "PHP_EXT_DIR block"
cat "$t" > "$f"

# 3. allowed USE_PHP extensions (random from 8.2, ffi from 7.4, sodium from 7.2)
awk '
  /^_USE_PHP_VER[0-9]+=/ && !done {
      print "_USE_PHP_VER56=\t${_USE_PHP_ALL:Nrandom:Nffi:Nsodium:Nzephir_parser} interbase mssql mysql opcache recode sybase_ct wddx"
      print "_USE_PHP_VER74=\t${_USE_PHP_ALL:Nrandom} bitset imagick opcache redis"
      done = 1 }
  { print }' "$f" > "$t"
grep -q '^_USE_PHP_VER74=' "$t" && grep -q '^_USE_PHP_VER56=' "$t" || die "_USE_PHP_VER"
cat "$t" > "$f"

# 4. extension dependencies that exist as separate ports in 5.6 / 7.4
awk '
  /^[.][[:space:]]+for extension in [$][{]USE_PHP[}]/ && !done {
      print ".    if ${PHP_VER} == 74"
      print "json_DEPENDS=\tdevel/php${PHP_VER}-json"
      print "openssl_DEPENDS=security/php${PHP_VER}-openssl"
      print "xmlrpc_DEPENDS=\tnet/php${PHP_VER}-xmlrpc"
      print ".    endif"
      print ".    if ${PHP_VER} == 56"
      print "hash_DEPENDS=\tsecurity/php${PHP_VER}-hash"
      print "interbase_DEPENDS=\tdatabases/php${PHP_VER}-interbase"
      print "json_DEPENDS=\tdevel/php${PHP_VER}-json"
      print "mcrypt_DEPENDS=\tsecurity/php${PHP_VER}-mcrypt"
      print "mssql_DEPENDS=\tdatabases/php${PHP_VER}-mssql"
      print "mysql_DEPENDS=\tdatabases/php${PHP_VER}-mysql"
      print "openssl_DEPENDS=security/php${PHP_VER}-openssl"
      print "recode_DEPENDS=\tconverters/php${PHP_VER}-recode"
      print "sybase_ct_DEPENDS=\tdatabases/php${PHP_VER}-sybase_ct"
      print "wddx_DEPENDS=\ttextproc/php${PHP_VER}-wddx"
      print "xmlrpc_DEPENDS=\tnet/php${PHP_VER}-xmlrpc"
      print ".    endif"
      print ""
      done = 1
  }
  { print }' "$f" > "$t"
grep -q '^json_DEPENDS=' "$t" || die "for extension loop"
cat "$t" > "$f"
rm -f "$t"

# 5. structure check: same number of .if/.for as .endif/.endfor
awk '/^[.][[:space:]]*(if|ifdef|ifndef|for)[[:space:]]/ {o++} /^[.][[:space:]]*(endif|endfor)/ {c++}
     END { if (o != c) { print "if/endif unbalanced: " o " vs " c > "/dev/stderr"; exit 1 } }' "$f" || die "structure"
echo "php.mk: PHP 5.6 and 7.4 inserted"
