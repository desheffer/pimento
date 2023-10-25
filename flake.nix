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
      in
        with pkgs; {
          devShells.default = mkShell {
            buildInputs = [
              (rust-bin.nightly.latest.default.override {
                extensions = [ "llvm-tools-preview" ];
                targets = [ "aarch64-unknown-none-softfloat" ];
              })
              cargo-binutils
              cargo-make
              qemu
            ];
          };
        }
    );
}
