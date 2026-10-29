# Created by: Wen Heping <wenheping@gmail.com>
# $FreeBSD$

PORTNAME=	intl
PORTVERSION=	3.0.0
PORTREVISION=	17
CATEGORIES=	devel pear

MAINTAINER=	sunpoet@FreeBSD.org
COMMENT=	PECL extension to support Internationalization(ICU)


LICENSE=	PHP301

LIB_DEPENDS=	libicui18n.so:devel/icu

IGNORE_WITH_PHP=	${_ALL_PHP_VERSIONS:N56}
USES=		compiler:c++17-lang php:pecl
USE_CXXSTD=	gnu++17

CPPFLAGS+=	-DU_USING_ICU_NAMESPACE=1

.include <bsd.port.mk>
