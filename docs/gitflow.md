# Git Flow

이 저장소는 Git Flow 브랜치 모델을 쓴다. 명령은 `git-flow-next`(git-flow의 현재 관리판)로 실행한다.

- 처음이면 [1부. 처음 쓰는 사람을 위한 안내](#1부-처음-쓰는-사람을-위한-안내)를 위에서부터 읽고 따라 해 본다.
- 쓰는 법을 알면 [2부. 참고](#2부-참고)에서 명령을 찾는다.

---

# 1부. 처음 쓰는 사람을 위한 안내

## Git Flow가 해결하는 문제

브랜치를 `main` 하나만 쓰면 이런 일이 생긴다.

- 반쯤 만든 기능이 `main`에 들어가 있어서, 지금 상태로는 배포(태그)를 할 수 없다.
- 배포한 버전에서 급한 버그가 나왔는데, 그 뒤에 들어간 미완성 코드 때문에 고친 버전만 따로 내기 어렵다.
- 어느 커밋이 실제로 배포된 것인지 헷갈린다.

Git Flow는 **"배포된 코드"와 "개발 중인 코드"를 다른 브랜치에 두고, 그 사이를 오가는 길을 정해 둔 규칙**이다.

| 비유 | 브랜치 |
|---|---|
| 가게 진열대. 손님(앱 개발자)이 가져가는 완성품만 놓는다 | `main` |
| 작업실 공용 작업대. 다음에 진열할 것들을 모아 맞춰 본다 | `develop` |
| 개인 작업 책상. 기능 하나를 만들고 다 되면 공용 작업대에 올린다 | `feature/…` |
| 진열 전 검수대. 가격표(버전)를 붙이고 마지막 점검을 한다 | `release/…` |
| 진열된 물건에 문제가 생겼을 때 바로 고치는 수리대 | `hotfix/…` |

`git flow` 명령은 이 규칙대로 브랜치를 만들고, 합치고, 지우고, 태그를 다는 일을 **명령 한 줄로** 대신해 준다. 일반 git 명령으로도 똑같이 할 수 있다([git flow가 대신 해 주는 일](#git-flow가-대신-해-주는-일)).

## 어떤 브랜치로 시작할까

| 지금 하려는 일 | 시작 명령 | 어디서 갈라져 어디로 합쳐지나 |
|---|---|---|
| 새 기능, 리팩터링, 문서 | `git flow feature start <이름>` | `develop` → `develop` |
| 아직 배포 안 된 코드(develop)의 버그 수정 | `git flow bugfix start <이름>` | `develop` → `develop` |
| 지금까지 모은 것을 새 버전으로 배포 | `git flow release start <버전>` | `develop` → `main` (태그 `v<버전>`, develop에도 반영) |
| **이미 배포된 버전**의 급한 버그 수정 | `git flow hotfix start <버전>` | `main` → `main` (태그 `v<버전>`, develop에도 반영) |

헷갈리면 "이 수정이 이미 배포된 코드에 있나?"를 묻는다. 있으면 hotfix, 없으면 bugfix다.

## 처음부터 따라 하기

실제 저장소를 건드리지 않도록 **빈 연습 저장소**에서 한다. 아래 출력은 git-flow-next 2.1.0으로 실제로 돌린 결과다. 그래프는 `git log --oneline --graph --all`로 본 모습이다(위가 최신).

설치가 안 돼 있으면 먼저 [설치](#설치)를 한다.

### 0. 연습 저장소 만들기

```sh
mkdir ~/gitflow-practice && cd ~/gitflow-practice
git init -b main
echo "# practice" > README.md
git add . && git commit -m "첫 커밋"
```

### 1. git flow 켜기

```sh
git flow init --preset=classic --defaults
git config gitflow.branch.release.tagprefix v
git config gitflow.branch.hotfix.tagprefix v
```

```
Initializing git-flow with classic preset
Created branch 'develop'
Git flow has been initialized
```

`develop` 브랜치가 생기고 그리로 이동했다. 아래 두 줄은 태그를 `0.1.0`이 아니라 `v0.1.0`으로 달게 하는 설정이다.

```sh
git branch
```
```
* develop
  main
```

### 2. 기능 하나 만들기

```sh
git flow feature start hello
```
```
Created branch 'feature/hello' from 'develop'
```

`feature/hello` 브랜치가 생겼고 그 위에 있다. 평소처럼 작업하고 커밋한다.

```sh
echo 'print("hello")' > hello.py
git add . && git commit -m "hello 추가"
```
```
* hello 추가 (HEAD -> feature/hello)
* 첫 커밋 (main, develop)
```

`main`과 `develop`은 그대로이고 내 브랜치만 앞서 있다.

### 3. 기능 끝내기

```sh
git flow feature finish --no-ff
```
```
Switched to branch 'develop'
Merging using strategy: merge
Successfully finished branch 'feature/hello' and updated 0 child base branches
You are now on branch 'develop'
```
```
*   Merge branch 'feature/hello' into develop (HEAD -> develop)
|\
| * hello 추가
|/
* 첫 커밋 (main)
```

일어난 일: `develop`으로 이동 → `feature/hello`를 병합 → `feature/hello` 브랜치 삭제. `main`은 아직 그대로다. 기능은 개발 중인 코드에만 들어갔다.

`--no-ff`를 빼면 병합 커밋 없이 `develop`이 `hello 추가`로 그냥 앞으로 당겨진다(fast-forward). 기능 단위 기록을 남기려고 붙였다.

### 4. 버전 0.1.0 배포하기

```sh
git flow release start 0.1.0
```
```
Created branch 'release/0.1.0' from 'develop'
```

릴리스 브랜치에서는 새 기능을 넣지 않는다. 버전 번호, 문서, 마지막 버그만 손본다.

```sh
echo "0.1.0" > VERSION
git add . && git commit -m "버전 0.1.0"
git flow release finish -m "Release 0.1.0"
```
```
Found child base branch 'develop' with auto-update enabled
Switched to branch 'main'
Merging using strategy: merge
Created tag 'v0.1.0'
Updating child base branch 'develop' from 'main'...
Using merge strategy for 'develop'
Successfully updated branch 'develop' from 'main'
Successfully finished branch 'release/0.1.0' and updated 1 child base branches
You are now on branch 'develop'
```
```
*   Merge branch 'main' into develop (HEAD -> develop)
|\
| * 버전 0.1.0 (tag: v0.1.0, main)
|/
*   Merge branch 'feature/hello' into develop
|\
| * hello 추가
|/
* 첫 커밋
```

일어난 일: `main`으로 이동 → 릴리스 브랜치 병합 → `v0.1.0` 태그 → `develop`이 `main`의 변경을 받음 → 릴리스 브랜치 삭제. 이제 `main`에 배포된 코드가 있고 태그가 붙었다.

### 5. 배포된 버전의 급한 수정

```sh
git flow hotfix start 0.1.1
echo 'print("hello!")' > hello.py
git commit -am "인사말 수정"
git flow hotfix finish -m "Hotfix 0.1.1"
```
```
*   Merge branch 'main' into develop (HEAD -> develop)
|\
| * 인사말 수정 (tag: v0.1.1, main)
* | Merge branch 'main' into develop
|\|
| * 버전 0.1.0 (tag: v0.1.0)
|/
...
```

핫픽스는 `develop`이 아니라 **`main`에서** 갈라진다. 그래서 `develop`에 미완성 기능이 있어도 고친 것만 `v0.1.1`로 낼 수 있다. 끝나면 `develop`에도 같은 수정이 들어간다.

### 6. 상태 확인

```sh
git tag
git flow overview
```
```
v0.1.0
v0.1.1
```

`git flow overview`의 마지막 `Active topic branches`에 진행 중인 feature·release·hotfix가 나온다. 지금은 모두 끝내서 `No active topic branches`다.

연습이 끝나면 `rm -rf ~/gitflow-practice`로 지운다.

## git flow가 대신 해 주는 일

git flow 명령은 일반 git 명령 여러 줄을 묶은 것이다. 아래처럼 직접 쳐도 결과는 같다.

| git flow 명령 | 같은 일을 하는 git 명령 |
|---|---|
| `git flow init` | `git branch develop main` + git-flow 설정 기록 |
| `git flow feature start x` | `git switch -c feature/x develop` |
| `git flow feature finish --no-ff` | `git switch develop`<br>`git merge --no-ff feature/x`<br>`git branch -d feature/x` |
| `git flow release start 0.1.0` | `git switch -c release/0.1.0 develop` |
| `git flow release finish -m "…"` | `git switch main`<br>`git merge release/0.1.0`<br>`git tag -a v0.1.0 -m "…"`<br>`git switch develop`<br>`git merge main`<br>`git branch -d release/0.1.0` |
| `git flow hotfix start 0.1.1` | `git switch -c hotfix/0.1.1 main` |

규칙을 외우지 않아도 되고, 순서를 빼먹거나(태그를 안 달거나 develop에 반영을 안 하거나) 할 일이 없다는 것이 git flow를 쓰는 이유다.

## 실수했을 때

| 상황 | 메시지·증상 | 해결 |
|---|---|---|
| 새로 받은 컴퓨터에서 바로 git flow 명령 | `Error: git flow is not initialized (run 'git flow init' first)` | [이 저장소에서 처음 한 번](#이-저장소에서-처음-한-번)의 명령 실행 |
| 같은 이름으로 다시 start | `Error: branch 'feature/x' already exists` | `git flow feature checkout x`로 이동하거나 다른 이름 사용 |
| 브랜치 이름을 잘못 지음 | — | 그 브랜치 위에서 `git flow rename <새 이름>` |
| feature를 안 만들고 `develop`에 바로 커밋함 (아직 push 안 함) | — | `git switch -c feature/<이름>`로 지금 커밋을 새 브랜치에 담고<br>`git switch develop && git reset --hard origin/develop`로 develop을 되돌림 |
| finish 중 충돌 | `Merge conflict detected while finishing feature/x` | 충돌 파일을 고치고 `git add <파일>`<br>`git flow feature finish x --continue`<br>(그만두려면 `git flow feature finish x --abort`) |
| 만들던 기능을 버림 | — | `git flow feature delete <이름>` (합치지 않고 지움) |

충돌이 나면 이미 `develop`으로 이동한 상태라서 짧은 명령 `git flow finish --continue`는 "주제 브랜치가 아니다"라며 동작하지 않는다. **종류와 이름을 붙인다**(`feature finish x --continue`).

---

# 2부. 참고

## 브랜치

| 브랜치 | 어디서 만드나 | 어디로 합치나 | 하는 일 |
|---|---|---|---|
| `main` | — | — | 배포된 버전만 있다. 배포마다 `vX.Y.Z` 태그 |
| `develop` | `main` | — | 다음 버전을 모으는 곳. 기능은 모두 여기로 합친다 |
| `feature/<이름>` | `develop` | `develop` | 기능 하나 |
| `bugfix/<이름>` | `develop` | `develop` | 아직 배포 안 된 코드의 버그 수정 |
| `release/<버전>` | `develop` | `main` (그리고 `develop`에 반영) | 배포 준비: 버전 올리기, 문서 정리. 끝내면 태그 |
| `hotfix/<버전>` | `main` | `main` (그리고 `develop`에 반영) | 배포된 버전의 급한 수정. 끝내면 태그 |
| `support/<이름>` | `main` | — | 옛 버전 유지보수(이 저장소는 쓰지 않는다) |

```
main     ──●───────────────────●────────────●──   (v0.1.0)  (v0.1.1)
            \                 / \          /
release      \       ●──●────●   \        /
              \     /             \      /
hotfix         \   /               ●────●
                \ /                      \
develop  ────────●───●───●────────●───────●──
                      \     /
feature                ●───●
```

## 설치

### macOS (Homebrew)

```sh
brew install git-flow-next
git flow version        # 2.1.0 (git-flow-next)
```

Homebrew에 있는 다른 git-flow는 쓰지 않는다.

| 패키지 | 상태 |
|---|---|
| `git-flow-next` | 현재 관리판. Go로 다시 만들었고 git-flow-avh 명령과 호환된다 |
| `git-flow` | 원조(nvie). 원본 저장소가 보관 처리돼 Homebrew에서 2026-12-19에 비활성화된다 |
| `git-flow-avh` | Homebrew에서 이미 빠졌다 |

### 다른 환경

- 리눅스·윈도우: https://git-flow.sh/ 의 설치 안내나 GitHub 릴리스(`gittower/git-flow-next`)의 바이너리를 쓴다.
- Go가 있으면 `go install github.com/gittower/git-flow-next@latest`. 실행 파일 이름이 `git-flow-next`로 생기므로 `git-flow`로 바꿔 `PATH`에 둔다(Homebrew판은 처음부터 `git-flow`).
- `git-flow`(원조)와 같은 실행 파일 이름을 쓰므로 둘을 함께 설치하지 않는다.

`git flow <명령>`과 `git-flow <명령>`은 같다. git은 `git-<이름>` 실행 파일을 `git <이름>`으로 불러 준다.

## 이 저장소에서 처음 한 번

git-flow 설정은 `.git/config`에 저장되고 커밋되지 않는다. **저장소를 새로 받은 컴퓨터마다** 한 번 실행한다.

```sh
git switch main && git pull
git flow init --preset=classic --defaults          # main, develop, feature/ … 기본 이름
git config gitflow.branch.release.tagprefix v      # 태그를 0.1.0 이 아니라 v0.1.0 으로
git config gitflow.branch.hotfix.tagprefix v
git flow overview                                  # 설정 확인
```

`develop`이 원격에 이미 있으면 `init`은 그것을 그대로 쓴다.

설정이 실제로 무엇인지는 `git config --get-regexp '^gitflow'`로 본다. 브랜치 종류마다 접두사(`prefix`), 갈라지는 곳(`startpoint`), 합치는 곳(`parent`), 태그 여부(`tag`) 등이 들어 있다.

## 명령어

### 상태 보기

| 명령 | 하는 일 |
|---|---|
| `git flow overview` | 기본 브랜치 관계와 진행 중인 주제 브랜치 목록 |
| `git flow config list` | 브랜치 종류별 설정(부모, 접두사, 태그 여부) |
| `git flow feature list` | 진행 중인 기능 브랜치. `release list`, `hotfix list`도 같다 |

### 기능 (feature)

| 명령 | 하는 일 |
|---|---|
| `git flow feature start <이름>` | `develop`에서 `feature/<이름>`을 만들고 이동 |
| `git flow feature publish` | 지금 기능 브랜치를 원격에 올린다(`git push -u`) |
| `git flow feature update` | `develop`의 새 커밋을 내 기능 브랜치에 가져온다(기본: rebase) |
| `git flow feature finish [이름]` | `develop`에 합치고 기능 브랜치를 지운 뒤 `develop`으로 이동 |
| `git flow feature checkout <이름>` | 다른 기능 브랜치로 이동 |
| `git flow feature track <이름>` | 원격에만 있는 기능 브랜치를 받아 온다 |
| `git flow feature rename <새 이름>` | 지금 기능 브랜치 이름 바꾸기 |
| `git flow feature delete <이름>` | 합치지 않고 지운다 |

`bugfix`도 명령이 같다(`git flow bugfix start <이름>` …).

### 릴리스 (release)

| 명령 | 하는 일 |
|---|---|
| `git flow release start <버전>` | `develop`에서 `release/<버전>`을 만든다. 버전은 `v` 없이 `0.1.0` |
| `git flow release publish` | 릴리스 브랜치를 원격에 올린다 |
| `git flow release finish -m "Release <버전>"` | `main`에 합치고 `v<버전>` 태그를 단 뒤 `develop`에도 반영하고 브랜치를 지운다 |

### 핫픽스 (hotfix)

| 명령 | 하는 일 |
|---|---|
| `git flow hotfix start <버전>` | `main`에서 `hotfix/<버전>`을 만든다. 예: `0.1.1` |
| `git flow hotfix finish -m "Hotfix <버전>"` | `main`에 합치고 `v<버전>` 태그를 단 뒤 `develop`에도 반영 |

### 지금 브랜치에 바로 쓰는 짧은 명령

주제 브랜치(feature·bugfix·release·hotfix) 위에서는 종류를 생략할 수 있다.

| 명령 | 같은 뜻 |
|---|---|
| `git flow finish` | `git flow <종류> finish` |
| `git flow publish` | `git flow <종류> publish` |
| `git flow update` | 부모 브랜치의 변경을 가져오기 |
| `git flow rename <새 이름>` / `git flow delete` | 이름 바꾸기 / 지우기 |

### finish에 자주 붙이는 옵션

| 옵션 | 하는 일 |
|---|---|
| `--no-ff` | fast-forward가 가능해도 병합 커밋을 만든다. **기본값은 fast-forward**라서 병합 기록을 남기려면 붙인다 |
| `-S`, `--squash` | 커밋을 하나로 합쳐 넣는다 |
| `-k`, `--keep` | 끝낸 뒤에도 브랜치를 지우지 않는다 |
| `--push` | 끝낸 뒤 대상 브랜치와 태그를 원격에 올린다 |
| `-m "<메시지>"` | 태그 메시지(release·hotfix) |
| `-n`, `--notag` | 태그를 만들지 않는다 |
| `--no-fetch` | 원격을 먼저 받아 오지 않는다(오프라인일 때) |
| `--continue` / `--abort` | 충돌 해결 후 이어 가기 / 그만두기. `git flow <종류> finish <이름> --continue`처럼 종류와 이름을 붙인다 |

## 이 저장소의 작업 순서

GitHub PR로 리뷰와 CI를 거친다. 그래서 기능은 로컬에서 `finish` 하지 않고 PR로 합친다.

### 기능 하나

```sh
git flow feature start input-validation          # develop에서 시작
# … 작업, 커밋 …
git flow feature publish                         # 원격에 올리기
gh pr create --base develop --fill               # develop 으로 PR
# GitHub에서 CI 통과 후 병합
git switch develop && git pull
git flow feature delete input-validation         # 로컬 브랜치 정리
```

혼자 빠르게 할 때는 PR 없이 `git flow feature finish --no-ff` 후 `git push`로 끝내도 된다.

### 버전 배포

```sh
git switch develop && git pull
git flow release start 0.2.0
# README·ROADMAP 의 버전과 체크 표시 정리, 커밋
git flow release finish -m "Release 0.2.0"       # main 병합, v0.2.0 태그, develop 반영
git push origin main develop --tags
```

태그가 올라가면 SwiftPM에서 `.package(url: …, from: "0.2.0")`으로 받을 수 있다.

### 배포된 버전의 급한 수정

```sh
git switch main && git pull
git flow hotfix start 0.2.1
# … 수정, 커밋 …
git flow hotfix finish -m "Hotfix 0.2.1"
git push origin main develop --tags
```

## 규칙

- `main`과 `develop`에 직접 커밋하지 않는다.
- 브랜치 이름은 영어 소문자와 `-`: `feature/cpa-tcpa`, `bugfix/nan-input`.
- 버전은 [유의적 버전](https://semver.org/lang/ko/). 1.0 전에는 깨는 변경도 부 버전(0.x)을 올린다.
- 태그는 `release`·`hotfix` finish가 단다. 손으로 태그를 만들지 않는다.
