# Copyright 1999-2016 Gentoo Foundation
# Distributed under the terms of the GNU General Public License v2
# $Header: $

EAPI=5

inherit mercurial qmake-utils

DESCRIPTION="Cloth patternmaking software"
HOMEPAGE="http://www.valentina-project.org/"
SRC_URI=""
EHG_REPO_URI="https://bitbucket.org/dismine/valentina"
EHG_REVISION="develop"

LICENSE="GPL-3"
SLOT="1"
KEYWORDS="~amd64 ~x86"

LANGS="ru_RU uk_UA de_DE cs_CZ he_IL fr_FR it_IT nl_NL id_ID es_ES fi_FI en_US en_CA en_IN ro_RO zh_CN"
for X in ${LANGS} ; do
	if [[ ${X} != "en" ]]; then
    	IUSE="${IUSE} linguas_${X}"
	fi
done

CDEPEND="
		dev-qt/qtcore:5
		dev-qt/qtgui:5
		dev-qt/qtwidgets:5
		dev-qt/qtxml:5
		dev-qt/qtsvg:5
		dev-qt/linguist:5
		dev-qt/qtxmlpatterns:5
		dev-qt/qtprintsupport:5
		dev-qt/qtnetwork:5
		app-text/popple"
RDEPEND="${CDEPEND}
DEPEND="${CDEPEND}
		dev-util/ccache"

src_configure() {
	eqmake5 Valentina.pro -r
}

src_install() {
	emake install INSTALL_ROOT="${D}"
#	dodoc Changelog.txt README.txt
}
