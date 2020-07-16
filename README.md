# securefs

`securefs`는 투명한 암호화 (쓰기시) 및 암호 해독 (읽기시) 이 있는 사용자 공간 (FUSE)의 파일 시스템입니다.

`securefs`는 일반 디렉토리를 마운트 지점에 마운트합니다. 마운트 지점은 파일, 디렉토리 및 심볼릭 링크를 읽거나 쓰거나 작성할 수있는 일반 파일 시스템으로 나타납니다. 기본 디렉토리는 암호화되고 인증 된 내용을 포함하도록 자동으로 업데이트됩니다.

## 동기

민감한 재무 기록부터 개인 일기, 유죄 쾌락 수집에 이르기까지 우리 모두는 개인의 눈을 사로 잡지 않도록해야 할 일이 있습니다. 특히 파일을 클라우드에 저장하면 회사와 NSA가 파일을 얻을 수 있습니다. 우리가 감당할 수있는 최선의 보호는 수학자와 군대가 원래 국가의 비밀을 지키기 위해 개발 한 학문 인 암호화입니다.

그러나 보안은 종종 편의와 상충되며 사람들은 쉽게 번거 로움을 느끼고 전혀 보호하지 않는 상태로 돌아갑니다. 로컬 또는 클라우드에서 파일을 보호하는 경우를 고려하십시오. 클라우드에 커밋하기 전에 파일을 암호화하고 읽고 쓸 때마다 해독해야합니다. 더 나쁜 것은, 그러한 행동은 하드 드라이브에 암호화되지 않은 흔적을 남깁니다. 클라우드에 데이터를 저장하면 수동 암호화 및 암호 해독으로 인해 파일이 효율적으로 동기화되지 않습니다.

`securefs`는 보안과 편의성이 충돌하지 않도록 가능한 한 원활하게 경험할 수 있도록 고안되었습니다. 가상 파일 시스템을 마운트 한 후 모든 것이 작동합니다.

## 비교

널리 사용되는 암호화 파일 시스템이 이미 많이 있습니다. 주목할만한 것은 TrueCrypt, FileVault, BitLocker, eCryptFS, encfs 및 gocryptfs입니다. Securefs는 다음 기능을 모두 갖춘 유일한 점이 다릅니다.
* [Authenticated encryption](https://en.wikipedia.org/wiki/Authenticated_encryption) (따라서 선택된 암호문 공격으로부터 보호)
* [Probabilistic encryption](https://en.wikipedia.org/wiki/Probabilistic_encryption) (따라서 의미 론적 보안을 제공합니다)
* 모든 주요 플랫폼에서 지원 (Mac, Linux, BSDs and Windows)
* 효율적인 클라우드 동기화 (컨테이너로 사전 할당 된 단일 파일이 아님)

## Install

[![Actions Status](https://github.com/netheril96/securefs/workflows/C%2FC%2B%2B%20CI/badge.svg)](https://github.com/netheril96/securefs/actions)


### macOS

Homebrew와 함께 설치하십시오. Osxfuse를 미리 설치해야합니다.
```
brew install securefs
```

### Windows

Windows 사용자는 릴리스 섹션에서 사전 빌드 된 패키지를 다운로드 할 수 있습니다. [WinFsp](https://github.com/billziss-gh/winfsp/releases) 및 [VC ++ 2017 재배포 패키지](https://support.microsoft.com/en-us/help/2977003/the-latest-supported-visual-c-downloads)에 에 의존 합니다.

### Linux

Linux 사용자는 소스에서 빌드해야합니다.

첫 번째 `fuse`를 설치해야합니다.

* 데비안 기반 Linux 배포판에서 `sudo apt-get install fuse libfuse-dev build-essential cmake`.
* RPM 기반 Linux에서, `sudo yum install fuse fuse-devel`.

그런 다음`git clone --recursive`로 소스를 복제하고`linux-build.sh`를 실행하십시오.

### FreeBSD (unofficial)

패키지를 사용하여 설치 (권하다):
```bash
pkg install fusefs-securefs
```

or ports:
```bash
make -C /usr/ports/sysutils/fusefs-securefs install
```

Make sure you load the fuse kernel module before using securefs:
```bash
kldload fuse
sysrc -f /boot/loader.conf fuse_load="YES"  # Load fuse automatically at boot
```

## 기본 사용법

*스왑 및 최대 절전 모드 파일을 비활성화하거나 암호화하는 것이 좋습니다. 그렇지 않으면 주 메모리에 저장된 일반 텍스트와 키는 언제든지 OS에 의해 디스크에 기록 될 수 있습니다.*

Examples:

```bash
securefs --help
securefs create ~/Secret
securefs chpass ~/Secret
securefs mount ~/Secret ~/Mount # press Ctrl-C to unmount
securefs m -h # m is an alias for mount, -h tell you all the flags
```

## 라이트 및 풀 모드

파일 시스템 형식에는 두 가지 범주가 있습니다.

**lite** 형식은 보안 성이 더 높지만`encfs` 작동 방식과 유사하게 파일 이름과 파일 내용을 개별적으로 암호화합니다.

**full** 형식은 가상 파일 시스템의 파일, 디렉토리 및 심볼릭 링크를 기본 파일 시스템의 일반 파일에 모두 매핑합니다. 디렉토리 구조가 평평 해지고 파일에 B-Tree로 기록됩니다.

라이트 형식은 특히 DropBox, Google Drive 등과 함께 사용하는 경우 Unix와 유사한 운영 체제에서 기본값이되어 훨씬 빨라지고 충돌 해결이 더 쉬워집니다. 그러나 전체 형식은 파일 시스템 계층 구조에 대한 정보가 더 적습니다. 기본 파일 시스템의 기능과 비교적 독립적이며 일반적으로 더 안전합니다.

더 이상 기본값이 아닌 전체 형식을 요청하려면`securefs create --format 2`를 실행하십시오.

## 설계 및 알고리즘

[여기](docs/design.md)를 보십시요.

## 경고

ICloud Drive에 'securefs'암호화 파일을 저장하면 iOS에서 Spotlight 검색이 작동하지 않을 수 있습니다. `securefs`가 아닌 iOS의 버그입니다.

이 버그를 해결하려면 Setting -> Siri 및 제안에서 *파일* 앱의 색인 생성을 비활성화 할 수 있습니다.
