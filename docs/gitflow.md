# Git Flow

이 저장소는 Git Flow 브랜치 모델을 쓴다. 명령은 `git-flow-next`(git-flow의 현재 관리판)로 실행한다.

## 브랜치

| 브랜치 | 어디서 만드나 | 어디로 합치나 | 하는 일 |
|---|---|---|---|
| `main` | — | — | 배포된 버전만 있다. 커밋마다 `vX.Y.Z` 태그 |
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

충돌이 나면 해결하고 `git add` 한 뒤 `git flow finish --continue`, 그만두려면 `git flow finish --abort`.

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
