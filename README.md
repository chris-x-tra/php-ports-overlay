# Re-inserts PHP 5.6.40 and 7.4.33 into the current FreeBSD ports upstream

* including the fix for CVE-2026-109618 and CVE-2026-109619 ("Avoid state corruption via bailout") for PHP7.4 and PHP5.6

* and the fix for CVE-2023-0662 (multipart request body DoS) for PHP5.6

~~~~
[root@sol ~]# freebsd-version
14.5-RELEASE-p3

[root@sol ~]# date
Mon Nov  2 13:37:00 CET 2026

[root@sol ~]# jls
   JID  IP Address      Hostname                      Path
    12  127.0.1.1       php74jail                     /usr/jails/php74jail
    13  127.0.2.1       php56jail                     /usr/jails/php56jail

[root@sol ~]# cd /usr/ports
[root@sol /usr/ports]# git pull
remote: Enumerating objects: 13, done.
remote: Counting objects: 100% (13/13), done.
remote: Compressing objects: 100% (6/6), done.
remote: Total 7 (delta 6), reused 2 (delta 1), pack-reused 0 (from 0)
Unpacking objects: 100% (7/7), 1.12 KiB | 76.00 KiB/s, done.
From https://git.FreeBSD.org/ports
   1d517f711349..b04f57dd8dca  main       -> origin/main
Updating 1d517f711349..b04f57dd8dca
Fast-forward
 devel/subversion/Makefile          | 4 ++++
 devel/subversion/files/svnserve.in | 1 +
 2 files changed, 5 insertions(+)
...

[root@sol /usr/local/src/php-ports-overlay]# PROFILE=php56 TARGET=/usr/jails/php56jail/usr/ports.php56/ \
 ./sync-ports.sh
php.mk: PHP 5.6 und 7.4 inserted
OK: /usr/jails/php56jail/usr/ports.php56/ = 8bc05c180ed7 (main) + overlay d97a478
php74 version: 7.4.33
php56 version: 5.6.40
[root@sol /usr/local/src/php-ports-overlay]#

[root@sol /usr/local/src/php-ports-overlay]# TARGET=/usr/jails/php74jail/usr/ports.php74/ ./sync-ports.sh
php.mk: PHP 5.6 und 7.4 inserted
OK: /usr/jails/php74jail/usr/ports.php74/ = 1d517f711349 (main) + overlay d97a478
php74 version: 7.4.33
php56 version: 5.6.40
[root@sol /usr/local/src/php-ports-overlay]#
~~~~
