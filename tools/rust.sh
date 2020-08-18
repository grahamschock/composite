curl https://sh.rustup.rs -sSf | sh -s -- -y
source $HOME/.cargo/env
rustup default nightly
cargo install xargo
rustup component add rust-src
