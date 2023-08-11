die() {
    echo "$@"
    exit 1
}

write_file() {
    fname="$1"
    shift
    for t in "$@"; do
        echo "$t" > "$fname"
    done
}

post_hook=()
run_at_exit() {
    post_hook+=("$@")
}
on_exit() {
    for((i = ${#post_hook[@]};i > 0;i--)); do
        ${post_hook[i]}
    done
}
trap on_exit EXIT

