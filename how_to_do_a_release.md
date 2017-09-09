# In case of Release #
*  ~~break glass~~
* Change the first heading, “UNRELEASED“, in the [changelog](./CHANGELOG.md) to the new version.
* Finish writing up the changelog if you slacked off during development of the version.
* Create a new git tag, adding the release notes of the version as comment: e.g. `git tag -a v0.2.1`
  * Note that headlines starting with # will be treated as comments and stripped from description if you don't use -m.
* Push the tag: e.g. `git push origin v0.2.1`
* Consider backporting stuff for Maintenance Releases
* Add new heading in the [changelog](./CHANGELOG.md) for the next release.
* Write blogpost about it by mainly copying the changelog and thanking contributors.

# In case of Maintenance Release #
* Create a branch for it without the patchlevel, e.g. `git checkout -b v0.2 v0.2.0`.
* cherry-pick bug fixes from newer versions.
