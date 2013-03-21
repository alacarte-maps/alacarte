Kurze Anleitung zu unserem "Workflow" mit git.

Wir haben 2 Haupt-Branches:
 * master
 * stable

master ist die instable Version. master wird in stable gemerged,
falls alle Tests laufen und im Allgemeinen alles in Butter ist.

Jeder erstellt, wenn er ein neues Feature implementieren will, einen eigenen Branch.

Bsp. Für einen Mock der Komponente Renderer:
Branch: mock-renderer

Ihr erstellt einen Branch mit:
	git branch master _EUER_BRANCH_
Also z.B. für mock-renderer:
	git branch master mock-renderer

Wie ihr schon seht, werden Feature-Branches immer von master aus abgezweigt und *nicht* von stable.

*Wichtig* oben habt ihr nur einen _lokalen_ Branch erzeugt. Falls ihr wollt,
das wir den auch sehen können macht folgendes:
	git push origin _EUER_BRANCH_
Beim mock-renderer also wieder:
	git push origin mock-renderer

Falls euer Feature fertig ist, kann dann per pull-request auf Bitbucket euer Zweig eingefügt werden.



# Arbeiten mit dem Feature-Branch

Ihr könnt auf eurem Branch wie gewohnt mit git arbeiten.
Also "git commit" und ähnliches funktioniert wie gewohnt.
*Vorsicht* wenn ihr pushen wollt:
	git push origin _EUER_BRANCH_
benutzen.

"Aber ich will jetzt ein paar neue Änderungen aus master in meinem Branch haben,
 weil _INSERT_REASON_!"
Okay. Aber die Sache ist nicht so einfach und in der Regel mit Arbeit (mergen!) verbunden.
Also sagen wir, du hast vor einer Woche deinen Branch erstellt die Git-History sieht jetzt so aus:

J  G
I  F
H /
E
D
C
B
A

D.h. du hast deinen Branch zu dem Zeitpunkt erstellt bei dem der Commit "E" der
neuste im master-Branch war und dort die Commits G und F gemacht.
In der Zwischenzeit wurden 3 gaaaanz tolle neue Commits H, I, J zum master
hinzugefügt, die du _unbedingt_ brauchst.
Nun gut, wie bekommen wir jetzt H, I, J in deinen Branch?
Wir tun einfach so, als ob du erst eben deinen Branch erzeugt hättest.
Genau das macht "git rebase". Also was wir später haben wollen:

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

Okay. Also einfach folgendes machen:
	git rebase master
Dieser Befehl erstellt aus eurem Branch eine Reihe von "Patches".
Dann checkt git den master-Branch aus und wendet diese Patches in der richtigen Reihenfolge an.

"Oh shit I broke something."
Ja. Beim rebasen kommt es öfter mal zu Fehlern. Aber Don´t Panic.
Falls ein Fehler aufgetreten ist macht folgendes:
	git status
Das sagt euch erst mal welche Dateien im Moment in Konflikt stehen.
Dann könnt ihr den Text-Editor euer Wahl öffnen und die Konflikte in dieser Datei per Hand lösen.
*Wichtig* guckt dabei _genau_ hin. Sonst können tolle Bug-Fixes wieder ganz schnell verloren gehen.
Falls ihr den Konflikt behoben habt:
	git add _KONFLIKT_DATEI_
Und macht mit der nächsten Datei weiter...
Irgendwann solltet ihr dann fertig sein und könnt folgendes tun:
	git rebase --continue
Dann nimmt sich git den nächsten Commit vor und ihr könnt vll wieder mergen, wer weiss.

"Aber, aber warum nicht 'git merge master'"
Weil wir keine Back-Merges haben wollen.
Kurz gesagt sieht unsere History sonst aus wie ein Wollkneul und nicht wie ein toller Baum.
(Zu dem Thema gibt es genug Diskussionen in der git-Welt aber your beloved Leader sagt jetzt einfach mal nein.)

# Den Feature-Branch von anderen Leuten ansehen

Mit folgenden Befehl könnt ihr gucken welche remote-Branches existieren:
	git branch -r

So könnt ihr euch einen lokalen Branch zum remote erstellen.
git checkout --track -b _BRANCH_NAME_ origin/_BRANCH_NAME

# Änderungen aus anderen Feature-Branches in den eigenen übernehmen.

Keine gute Idee. Wirklich. Kann später beim mergen zu bösen Kopfschmerzen führen,
besonders wenn anschließend noch mal ein git-rebase ausgeführt wurde.
Die Feature-Branches sollten so ausgelegt werden, das man genau das nicht machen muss.

# Einen einzelnen Commit aus eurem Feature-Branch in master übernehmen

Das ist durchaus machbar und wenn der Commit sauber getrennt ist von eurer restlichen
Arbeit ohne große Nebenwirkungen. Was ihr dazu tun müsst:

Zuerst brauchen wir die ID eures Commits:
	git log

Die Ausgabe sieht ja dann ca. so aus:
	commit 7ad59a81e1a83ac752760d3436222de0d93b8480
	Author: Bernhard Scheirle <bernhard.scheirle@student.kit.edu>
	Date:   Thu Dec 27 13:54:23 2012 +0100

    	Transform: Set comments to doxygen comments.

Der kryptische Wert nach "commit" ein Hash-Wert die ID des Commits, hier also:
	7ad59a81e1a83ac752760d3436222de0d93b8480

Was ihr nun tun müsst:
	git checkout master
	git cherry-pick _COMMIT_ID_

Für unseren Beispiel-Commit also:
	git cherry-pick 7ad59a81e1a83ac752760d3436222de0d93b8480

Jetzt wechselt ihr zu eurem Feature-Branch und rebased auf master:
	git checkout _EURER_BRANCH_
	git rebase master

So, das sollte jetzt in der Regel funktionieren. Allerdings kann es natürlich wieder
zu Konflikten kommen, aber wie man damit umgeht steht ja oben.

# Tip und Tricks für eine Saubere History

## Teil-Commits
Mein lieblings Befehl ist:
	git add --patch

Dieses nette Feature erlaubt es euch *genau* zu bestimmen welche Änderungen
übernommen werden sollen in einem Commit.
Extrem praktisch wenn man wärend man Feature A implementiert hat auch noch einen
Bug gefixt hat und das in einem eigenen Commit haben will.

## Vergessene Änderungen
Es passiert häufiger das man beim letzten Commit vergessen hat, was recht einfach
zu fixen ist.
Einfach noch mal sowas machen wie:
	git add _DIE_VERGESSENE_DATEI_
Und dann ein:
	git commit --amend

Damit wird euer letzter commit mit euren neu hinzugefügten Änderungen zusammengeführt.


