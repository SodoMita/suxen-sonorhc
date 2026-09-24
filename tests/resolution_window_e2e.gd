extends Node
## Window-level resolution check. Run under a real window manager (see
## tests/resolution_window_test.sh): a headless dummy window cannot be
## maximized or fullscreen, which is exactly where this setting used to die.
##
## Drives the shipped settings panel and reports the window, the panel widgets
## and the toast, in every window state.

var failures: int = 0
var balloon: Node


## What the window should become for a request, worked out here instead of by
## asking DisplayScale: the test has to state the expectation on its own, or it
## would agree with whatever the shipped fit_to_screen() happens to do.
func _expected(w: int, h: int) -> Vector2i:
	var usable := DisplayServer.screen_get_usable_rect(DisplayServer.window_get_current_screen())
	var work := usable.size
	if work.x < 2 or work.y < 2:
		work = DisplayServer.screen_get_size(DisplayServer.window_get_current_screen())
	if w < 1 or h < 1 or work.x < 2 or work.y < 2:
		return Vector2i(w, h)
	var fit := minf(work.x / float(w), work.y / float(h))
	if fit >= 1.0:
		return Vector2i(w, h)
	return Vector2i(maxi(int(w * fit), 1), maxi(int(h * fit), 1))


func _check(label: String, ok: bool, detail := "") -> void:
	if not ok:
		failures += 1
	print("%s  %-46s %s" % ["PASS" if ok else "FAIL", label, detail])


func _win() -> Vector2i:
	return DisplayServer.window_get_size()


func _win_state(label: String) -> void:
	var root := get_tree().root
	print("      %-30s win=%-12s mode=%d px_per_unit=%.3f" % [label, str(_win()),
		get_window().mode, root.get_final_transform().get_scale().x])


func _panel() -> String:
	var opt: OptionButton = balloon.get_node("%ResolutionOption")
	var sw: SpinBox = balloon.get_node("%ResWidthSpin")
	var sh: SpinBox = balloon.get_node("%ResHeightSpin")
	return "%s [%dx%d] disabled=%s" % [opt.get_item_text(opt.selected), int(sw.value), int(sh.value),
		str(opt.is_item_disabled(opt.selected))]


func _toast() -> String:
	var t: Label = balloon.get_node("%ToastLabel")
	return t.text if t.visible else "(none)"


func _pick(i: int) -> void:
	balloon.call("_on_resolution_selected", i)


func _ready() -> void:
	await get_tree().process_frame
	var screen := DisplayServer.screen_get_size()
	print("### screen=", screen, " work_area=", DisplayServer.screen_get_usable_rect().size)
	balloon = load("res://scenes/vn_balloon.tscn").instantiate()
	add_child(balloon)
	for i in range(6):
		await get_tree().process_frame
	# A previous run may have saved a size this screen has to fit, and that
	# leaves a toast up. Clear it so the checks below see only their own.
	balloon.get_node("%ToastTimer").stop()
	balloon.get_node("%ToastLabel").hide()
	_win_state("panel opened")
	print("      panel shows: ", _panel())

	# 1. A preset that fits must change the window.
	_pick(0)                      # 1280x720
	await get_tree().process_frame
	await get_tree().process_frame
	var after_720 := _win()
	_check("preset 1280x720 applies", after_720 == Vector2i(1280, 720), str(after_720))
	_check("no toast while the pick fits", _toast() == "(none)", _toast())

	var big := 2 if _expected(1920, 1080) == Vector2i(1920, 1080) else 1
	var big_asked := Vector2i([1280, 1600, 1920][big], [720, 900, 1080][big])
	_pick(big)
	await get_tree().process_frame
	await get_tree().process_frame
	_win_state("after preset %d" % big)
	_check("preset %s applies (fitted if needed)" % str(big_asked),
		_win() == _expected(big_asked.x, big_asked.y),
		"win=%s expected=%s" % [str(_win()), str(_expected(big_asked.x, big_asked.y))])

	# 2. A preset bigger than the screen: window fits, panel stops lying, toast explains.
	var last := 3                                   # 2560x1440
	var want_fit: Vector2i = _expected(2560, 1440)
	_pick(last)
	await get_tree().process_frame
	await get_tree().process_frame
	_win_state("after 2560x1440")
	_check("oversized preset fits the screen", _win() == want_fit, "win=%s fitted=%s" % [str(_win()), str(want_fit)])
	print("      panel now: ", _panel())
	_check("panel reports the size really applied", _panel().contains("%dx%d" % [want_fit.x, want_fit.y]), _panel())
	_check("toast explains the fit", _toast().contains("%d x %d" % [want_fit.x, want_fit.y]), _toast())

	# 3. Custom size.
	balloon.get_node("%ResWidthSpin").value = 1000.0
	balloon.call("_on_res_width_changed", 1000.0)
	await get_tree().process_frame
	await get_tree().process_frame
	_win_state("custom 1000x...")
	_check("custom width applies", _win().x == 1000, str(_win()))

	# 4. Maximized: the pick must leave that state and take effect.
	get_window().mode = Window.MODE_MAXIMIZED
	for i in range(10):
		await get_tree().process_frame
	_win_state("maximized (before pick)")
	var before := _win()
	_pick(0)
	for i in range(10):
		await get_tree().process_frame
	_win_state("maximized + 1280x720 pick")
	_check("pick leaves maximized", get_window().mode == Window.MODE_WINDOWED, "mode=%d" % get_window().mode)
	_check("pick resizes a maximized window", _win() == Vector2i(1280, 720) and _win() != before, str(_win()))
	var fs: CheckBox = balloon.get_node("%FullscreenCheck")
	_check("fullscreen box stays off", not fs.button_pressed, "pressed=%s" % str(fs.button_pressed))

	# 5. Fullscreen: same rule.
	get_window().mode = Window.MODE_FULLSCREEN
	for i in range(10):
		await get_tree().process_frame
	_win_state("fullscreen (before pick)")
	_pick(1)
	for i in range(10):
		await get_tree().process_frame
	_win_state("fullscreen + preset pick")
	var fs_want: Vector2i = _expected(1600, 900)
	_check("pick leaves fullscreen", get_window().mode == Window.MODE_WINDOWED, "mode=%d" % get_window().mode)
	_check("pick resizes a fullscreen window", _win() == fs_want, "win=%s expected=%s" % [str(_win()), str(fs_want)])

	# 6. A restored size must NOT fight a fullscreen window (only a pick may).
	get_window().mode = Window.MODE_FULLSCREEN
	for i in range(10):
		await get_tree().process_frame
	var fs_before := _win()
	balloon.call("_apply_resolution", 1600, 900)      # restore path, force = false
	for i in range(10):
		await get_tree().process_frame
	_check("restore keeps fullscreen", get_window().mode == Window.MODE_FULLSCREEN, "mode=%d" % get_window().mode)
	_check("restore leaves the fullscreen window size alone", _win() == fs_before,
		"before=%s after=%s" % [str(fs_before), str(_win())])
	_win_state("restore while fullscreen")

	# 7. Presets the screen cannot show are shown as unavailable.
	var opt: OptionButton = balloon.get_node("%ResolutionOption")
	for i in range(4):
		var asked := Vector2i([1280, 1600, 1920, 2560][i], [720, 900, 1080, 1440][i])
		var fits: bool = _expected(asked.x, asked.y) == asked
		_check("preset %s %s on a %s screen" % [str(asked), "selectable" if fits else "greyed out",
			str(DisplayServer.screen_get_usable_rect().size)],
			opt.is_item_disabled(i) == not fits, "disabled=%s" % str(opt.is_item_disabled(i)))

	print("=== e2e done: ", failures, " failure(s) ===")
	get_tree().quit(1 if failures > 0 else 0)
