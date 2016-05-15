The Git Guide
=============

This is a short introduction to our git workflow.

Our main development branch is `master`, and stable versions are tagged on release. If it's neccessary to change them
later, we would create a new branch from the release tag.

Everybody creates a new branch for a new feature he wants to implement.
For example, we want to add a mock of the `Renderer` component, we call our feature branch `mock-renderer`.

You create the new branch with `git branch master <YOUR_BRANCH>`.
So, for the `mock-renderer`, it's `git branch master mock-renderer`.
As you are already seeing, feature branches branch of from `master`, and not from the stable branches.

*Important*: You just created a _local_ Branch. If you want others to see it, do a `git push origin <YOUR_BRANCH>`.
So for the `mock-renderer`, do a `git push origin mock-renderer`.

As soon as your feature branch is finished, create a pull request for it to be merged in master.

## Working with the feature branch

You can work on the feature branch with git as you're used to do.
So `git commit` and similar works as excepted.

But *watch out!* in case you want to push: Use `git push origin <YOUR_BRANCH>`!

“But I want a few changes from master in my branch now, because *INSERT_REASON*!”
Okay. But that's not that simple, and usually associated with work (merging!).
So let's say, you created a feature branch a week ago. The git history now looks like this:

```
J  G
I  F
H /
E
D
C
B
A
```

This means you created the branch at the point in time where the commit “E“ was the newest in the master branch,
and you did the commits “G“ and “F“.

In the meantime, there were 3 veery awesome commits named “H”, “I“ and “J“ added to master
that you _absolutely_ need.

Well, now, how do we get “H”, “I” and “J” in your branch?
We pretend that you created your branch just now!

That's exactly what `git rebase` does. So what we want to have later is:

```
   G
   F
  /
J
I
H
E
D
C
B
A
```

Okay. So let's do that! Do a `git rebase master`!

This command creates a number of “patches” from your branch. Then it checks out the master branch and applies these
patches in the right order.

	“Oh shit I broke something.”

Yes. While rebasing, there are errors now and then. But don´t panic.

In case of an error, do a `git status`. That tells you which files are in conflict at the moment.
Then you can start the text editor of your choice and solve the conflicts in that file by hand.

*Important:* Please look _really_ closely. Otherwise, awesome bug fixes can be lost pretty damn quick.

As soon as you solved the conflicts, execute `git add <CONFLICTING_FILE>`.
And then continue with the next file…

Eventually, you should be finished and are able to do a `git rebase --continue`.
Then git will set out to take the next commit and mabye you'll need to merge again, who knows?

	“But, why not git merge master?”

Because we don't want to have back merges.

In short, our history will start to look like a ball of whool and not like a terific tree.

(Regarding this topic, there is much discussion in the git world, but your your beloved leader Patrick just says no for now.)


## Look at the feature branch of other people


You can see all existing remote branches with `git branch -r`.

You can then create a local branch to that remote branch with
`git checkout --track -b _BRANCH_NAME_ origin/_BRANCH_NAME`.



## Transfer changes from other feature branches to your feature branch

Not a good idea. Really. Could lead to severe headaches when merging later on, especially when a subsequent `git rebase`
was exectude. The feature branches should be planned in a way so that this does not have to be done.


## Apply a single commit from your feature branch to master

That's indeed feasible when the commit is cleanly separated from your remaining other work and without big side effects.
What you have to do for that:

First, you need the ID of your commit:
```
	git log
```

The output should look something like this:
```
	commit 7ad59a81e1a83ac752760d3436222de0d93b8480
	Author: Bernhard Scheirle
	Date:   Thu Dec 27 13:54:23 2012 +0100

	Transform: Set comments to doxygen comments.
```

The cryptic value after “commit” is a hash value, the ID of the commit. In this case, it's
`7ad59a81e1a83ac752760d3436222de0d93b8480`.

Now, do the following:
```
	git checkout master
	git cherry-pick <COMMIT_ID>
```

For our example, that's:
```
	git cherry-pick 7ad59a81e1a83ac752760d3436222de0d93b8480
```

Next, switch to your feature branch and rebase from master:
```
	git checkout <YOUR_BRANCH>
	git rebase master
```

That should do the trick nine times out of ten. Of course this can lead to conflicts, but how you can cope with them was
already explained above.


## Tip & Tricks for a clean History

### Partial Commits

My favourite command is `git add --patch`.
This nice little feature allows you to define *exactly* which changes you want to be added to your commit.

Extremely handy when you, while implementing feature A, also fixed a bug and you want to have that in a separate commit.

### Missing Changes
Occasionally it happens that you forgot something that should have belonged in the last commit.
That's very easy to fix, though!

Just do a: `git add <THE_FORGOTTEN_FILE>` and then a `git commit --amend`.
With that, your last commit will be combined with your newly-added changes.
