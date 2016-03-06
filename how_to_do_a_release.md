# In case of Release #
*  ~~break glass~~
* Change the first heading, “UNRELEASED“, in the [changelog][./CHANGELOG.md] to the new version.
* Finish writing up the changelog if you slacked off during development of the version.
* Create a new git tag, adding the release notes of the version as comment: e.g. `git tag -a v0.2.1`
* Push the tag: e.g. `git push origin v0.2.1`
* In case of major release: Create a branch for it without the patchlevel, e.g. `v0.2`.
* Add new heading in the [changelog][./CHANGELOG.md] for the next release.
* Write blogpost about it by mainly copying the changelog and thanking contributors.
