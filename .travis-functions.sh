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

function script_xvfb_run
{
	# start xvfb X server and give it some time to start
	export DISPLAY=:99.0
	if [ "${TRAVIS_OS_NAME}" = "osx" ]; then
		sudo Xvfb :99 -ac -screen 0 1024x768x24 &
	else
		Xvfb :99 -ac -screen 0 1024x768x24 &
	fi
	sleep 3

	# just for info output
	glewinfo

	# Run the program.
	#
	# For now we just start it in background and see if it's still running. Once
	# it's fixed for OSX and Ubuntu soft-GL we could use paraameters like
	#   --screenshot ../test/data/surfR_p_dist.ply ../test/data/distR.labels
	# Note, OSX Xvfb has GL Version 4.1, so it should be possible somehow ...

	./bin/DirectionalityIndicator &
	sleep 10

	# kill returns failure if there is no process running
	kill $!
}

function script_generic
{
	xconfigure || return
	$MAKE || return
	script_xvfb_run || return
}

function install_deps_linux
{
	# install some packages from Ubuntu's default sources
	sudo apt-get -qq update

	# we should better install the glewinfo version from our bundled sources
	sudo apt-get install -qq \
		glew-utils \
		|| return

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
		glew \
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
