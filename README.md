# P2PFileSync

## Proposal


## Usage

### Possible arguments

- --host:
- --mount_point:

## Todo

- [ ] Switch to google tcmalooc

## Compile Requirement

### Source Dependencies

For MacOS:

```shell
brew install pkg-config
```

For Ubuntu/Debian:

```shell
sudo apt-get install -y pkg-config
```

These Dependencies will automatically download via vcpkg or git submodule

- vcpkg
- bundle
- glog
- gtest
- gflags
- protobuf

### Package Dependencies

These Dependencies need to be install manually in order to compile

- libfuse (under linux)
- osx-fuse (under macos)

