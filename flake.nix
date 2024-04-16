{
  inputs = {
    nixpkgs.url = "github:nixos/nixpkgs/nixos-unstable";
    rust-overlay.url = "github:oxalica/rust-overlay";
    flake-utils.url = "github:numtide/flake-utils";
  };

  outputs = { self, nixpkgs, rust-overlay, flake-utils, ... }:
    flake-utils.lib.eachDefaultSystem (system:
      let
        overlays = [ (import rust-overlay) ];

        pkgs = import nixpkgs {
          inherit system overlays;
        };

        crossPkgs = import nixpkgs {
          inherit system overlays;

          crossSystem = "aarch64-linux";
        };

      in
        with pkgs; {
          devShells.default = mkShell {
            nativeBuildInputs = [
              (crossPkgs.buildPackages.rust-bin.selectLatestNightlyWith (toolchain: toolchain.default.override {
                extensions = [ "llvm-tools-preview" ];
                targets = [ "aarch64-unknown-none-softfloat" ];
              }))
              cargo-binutils
              cargo-make
              entr
              qemu
            ];
          };
        }
    );
}
