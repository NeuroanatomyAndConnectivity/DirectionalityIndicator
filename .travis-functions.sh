#!/bin/bash

#
# .travis-functions.sh:
#   - helper functions to be sourced from .travis.yml
#   - designed to respect travis' environment but testing locally is possible
#

if [ ! -f "src/CMakeLists.txt" ]; then
	echo ".travis-functions.sh must be sourced from source dir" >&2
	return 1 || exit 1
fi

# some config settings
MAKE="make -j2"

function xconfigure
{
	local CMAKEOPT

	if test "$USE_QT" = "qt4"; then
		CMAKEOPT+=" -DDI_FORCE_QT4=ON"
	fi
	cmake $CMAKEOPT ../src
}

function script_generic
{
	xconfigure || return
	$MAKE || return
}

function install_deps_linux
{
	# install some packages from Ubuntu's default sources
	sudo apt-get -qq update

	if test "$USE_QT" = "qt4"; then
		sudo apt-get install -qq \
			libqt4-dev \
			libqt4-opengl-dev \
			|| return
	else
		sudo apt-get install -qq \
			qt5-default \
			libqt5opengl5-dev \
			|| return
	fi
}

function install_deps_osx
{
	brew update >/dev/null
	brew install \
		qt5 \
		|| return

	QT_DIR=$(brew --prefix qt5)
	PATH="$QT_DIR/bin:$PATH"
}

function travis_install_script
{
	if [ "$TRAVIS_OS_NAME" = "osx" ]; then
		install_deps_osx || return
	else
		install_deps_linux || return
	fi
}

function travis_script
{
	local ret

	# cd causes ugly traces on OSX, thats why we do it here
	cd build || return

	set -o xtrace

	script_generic
	ret=$?

	set +o xtrace
	return $ret
}
