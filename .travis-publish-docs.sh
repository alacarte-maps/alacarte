#!/bin/bash

# GH_TOKEN is a github personal OAUTH tokens, create it here:
# https://github.com/settings/tokens/new (only needs “repo“ access.)
# Then, add them as hidden environment variable in the Travis project settings.
if [ "${TRAVIS_BRANCH}" = "master" ] && ["${TRAVIS_PULL_REQUEST}" = "false"]; then
	ghp-import -n -m "Updated documentation from ${TRAVIS_COMMIT}" doc/doxygen/html
	git config user.name "Travis CI"
	git config push.default simple
	git push -fq "https://${GH_TOKEN}@github.com/alacarte-maps/alacarte.git" gh-pages
	echo "documentation was pushed."
fi

# TODO: it would be nice to be able to publish the docs of multiple branches in subfolders, e.g. master and stable
