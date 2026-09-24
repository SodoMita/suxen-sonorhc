# Workflow files waiting for a `workflow`-scoped token

Pushing anything under `.github/workflows/` needs a token with the `workflow`
scope; a GitHub App token never has it, and the personal token used for the
resolution work had only `Contents: read and write`. Files land here first, then
move by hand (see the history of `ci/godot-ci.yml`).

## `window-tests-job.yml`

Adds the `window-tests` job to `.github/workflows/godot-ci.yml`: the
resolution setting can only fail under a real window manager, so the job runs
`tests/resolution_window_test.sh` under Xvfb + openbox with xdotool.

* paste the job into `jobs:` in `.github/workflows/godot-ci.yml`, or
* `git apply ci/window-tests.patch` from the repository root.

Either way, commit and push with a `workflow`-scoped token.

Until it lands, the test still runs by hand:

```bash
sudo apt-get install -y xvfb openbox xdotool
GODOT_BIN=/path/to/godot bash tests/resolution_window_test.sh
```
