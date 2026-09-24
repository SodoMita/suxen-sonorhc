extends RefCounted
## Shared window layout for the game and the panic page.
## The layout stays at its authored 1280x720 geometry (the UI chrome, the
## system row and the title card are all laid out in these units). A larger
## window draws that layout with more pixels instead of stretching a
## low-resolution picture.


const DESIGN := Vector2i(1280, 720)


## How much a window enlarges the design canvas. Below the design size, leave
## it at 1 so a narrow window reflows instead of shrinking the chrome.
static func keep_ratio(window_size: Vector2) -> float:
	if window_size.x < 1.0 or window_size.y < 1.0:
		return 1.0
	var fit := minf(window_size.x / float(DESIGN.x), window_size.y / float(DESIGN.y))
	return fit if fit > 1.0 else 1.0


static func read_settings() -> Dictionary:
	# Chrono Nexus keeps its settings namespaced away from the demo balloon
	# this UI was adapted from.
	if not FileAccess.file_exists("user://chrono_nexus/settings.json"):
		return {}
	var parsed: Variant = JSON.parse_string(FileAccess.get_file_as_string("user://chrono_nexus/settings.json"))
	return parsed if parsed is Dictionary else {}


## Pin the layout to the design size and give the extra pixels to the window.
## Headless has no window; mutating the layout there breaks orientation tests.
static func apply_window(tree: SceneTree, w: int, h: int) -> void:
	if w < 1 or h < 1 or tree == null:
		return
	if DisplayServer.get_name() == "headless":
		return
	var root := tree.root
	root.content_scale_mode = Window.CONTENT_SCALE_MODE_CANVAS_ITEMS
	root.content_scale_aspect = Window.CONTENT_SCALE_ASPECT_EXPAND
	root.content_scale_size = DESIGN
	# A saved resolution can be larger than the screen. canvas_items needs
	# only the window's pixel density — the layout never changes — so clamp
	# the OS window to the work area instead of spawning an oversized window
	# the window manager cannot handle.
	var usable := DisplayServer.screen_get_usable_rect(DisplayServer.window_get_current_screen())
	if usable.size.x > 1 and usable.size.y > 1:
		var fit := minf(usable.size.x / float(w), usable.size.y / float(h))
		if fit < 1.0:
			w = maxi(int(w * fit), 1)
			h = maxi(int(h * fit), 1)
	# Leave a maximized window alone: the pin above is all it needs.
	if root.mode == Window.MODE_MAXIMIZED or root.mode == Window.MODE_FULLSCREEN:
		return
	DisplayServer.window_set_size(Vector2i(w, h))
