#include "gdextension_interface.h"
#include "mix.h"

#include <string.h>

#if defined(_WIN32)
#define SCENE_SCORE_EXPORT __declspec(dllexport)
#else
#define SCENE_SCORE_EXPORT __attribute__((visibility("default")))
#endif

typedef struct {
	uint8_t data[16];
} NameBuf;

typedef struct {
	uint8_t data[24];
} VarBuf;

static struct {
	GDExtensionInterfaceGetGodotVersion2 get_godot_version2;
	GDExtensionInterfaceMemAlloc mem_alloc;
	GDExtensionInterfaceMemFree mem_free;
	GDExtensionInterfacePrintError print_error;
	GDExtensionInterfaceClassdbRegisterExtensionClass6 classdb_register_extension_class6;
	GDExtensionInterfaceClassdbUnregisterExtensionClass classdb_unregister_extension_class;
	GDExtensionInterfaceClassdbRegisterExtensionClassMethod classdb_register_extension_class_method;
	GDExtensionInterfaceClassdbConstructObject3 classdb_construct_object3;
	GDExtensionInterfaceObjectSetInstance object_set_instance;
	GDExtensionInterfaceStringNameNewWithUtf8Chars string_name_new_with_utf8_chars;
	GDExtensionInterfaceStringNewWithUtf8Chars string_new_with_utf8_chars;
	GDExtensionInterfaceVariantNewNil variant_new_nil;
	GDExtensionInterfaceVariantDestroy variant_destroy;
	GDExtensionInterfaceVariantGetType variant_get_type;
	GDExtensionInterfaceVariantCall variant_call;
	GDExtensionInterfaceGetVariantFromTypeConstructor get_variant_from_type_constructor;
	GDExtensionInterfaceGetVariantToTypeConstructor get_variant_to_type_constructor;
	GDExtensionInterfaceVariantGetPtrInternalGetter variant_get_ptr_internal_getter;
	GDExtensionInterfacePackedFloat64ArrayOperatorIndex packed_float64_array_operator_index;
	GDExtensionInterfacePackedVector2ArrayOperatorIndex packed_vector2_array_operator_index;
	GDExtensionVariantFromTypeConstructorFunc from_bool;
	GDExtensionVariantFromTypeConstructorFunc from_int;
	GDExtensionTypeFromVariantConstructorFunc to_float;
	GDExtensionTypeFromVariantConstructorFunc to_int;
	GDExtensionVariantGetInternalPtrFunc packed_f64_ptr;
	GDExtensionVariantGetInternalPtrFunc packed_v2_ptr;
} api;

static GDExtensionClassLibraryPtr library_ptr;
static NameBuf sn_class;
static NameBuf sn_parent;
static NameBuf sn_empty;
static NameBuf sn_size;
static NameBuf sn_transition;
static NameBuf sn_render_into;
static NameBuf sn_adjust;
static NameBuf sn_reseed;
static NameBuf sn_release;
static NameBuf sn_set_gain;
static NameBuf sn_active;
static NameBuf sn_notes;
static NameBuf sn_params;
static NameBuf sn_fade;
static NameBuf sn_score_id;
static NameBuf sn_seed;
static NameBuf sn_frames;
static NameBuf sn_gain;
static NameBuf empty_string;
static int registered = 0;

static void make_name(NameBuf *name, const char *text) {
	memset(name, 0, sizeof(*name));
	api.string_name_new_with_utf8_chars(name, text);
}

static void make_string(NameBuf *name, const char *text) {
	memset(name, 0, sizeof(*name));
	api.string_new_with_utf8_chars(name, text);
}

static void set_ok(GDExtensionCallError *error) {
	if (error) {
		error->error = GDEXTENSION_CALL_OK;
		error->argument = 0;
		error->expected = 0;
	}
}

static void return_nil(GDExtensionVariantPtr ret, GDExtensionCallError *error) {
	api.variant_new_nil(ret);
	set_ok(error);
}

static void return_bool(GDExtensionVariantPtr ret, int value, GDExtensionCallError *error) {
	uint8_t bit = value ? 1 : 0;
	api.from_bool(ret, &bit);
	set_ok(error);
}

static void return_int(GDExtensionVariantPtr ret, int64_t value, GDExtensionCallError *error) {
	api.from_int(ret, &value);
	set_ok(error);
}

static void fail_arg(GDExtensionVariantPtr ret, GDExtensionCallError *error, int32_t argument, int32_t expected) {
	api.variant_new_nil(ret);
	if (error) {
		error->error = GDEXTENSION_CALL_ERROR_INVALID_ARGUMENT;
		error->argument = argument;
		error->expected = expected;
	}
}

static int64_t call_size(GDExtensionConstVariantPtr packed) {
	VarBuf ret;
	GDExtensionCallError error;
	int64_t count = -1;
	memset(&error, 0, sizeof(error));
	api.variant_call((GDExtensionVariantPtr)packed, &sn_size, 0, 0, &ret, &error);
	if (error.error == GDEXTENSION_CALL_OK) {
		api.to_int(&count, &ret);
		api.variant_destroy(&ret);
	}
	return count;
}

static int read_params(GDExtensionConstVariantPtr packed, double *out) {
	int64_t count;
	int64_t i;
	void *array;
	if (api.variant_get_type(packed) != GDEXTENSION_VARIANT_TYPE_PACKED_FLOAT64_ARRAY) {
		return -1;
	}
	count = call_size(packed);
	if (count < 0) {
		return -1;
	}
	if (count > SCENE_SCORE_PARAMS) {
		count = SCENE_SCORE_PARAMS;
	}
	array = api.packed_f64_ptr((GDExtensionVariantPtr)packed);
	for (i = 0; i < count; i++) {
		double *slot = (double *)api.packed_float64_array_operator_index(array, i);
		out[i] = slot ? *slot : 0.0;
	}
	return (int)count;
}

static double read_float(GDExtensionConstVariantPtr value) {
	double out = 0.0;
	if (api.variant_get_type(value) != GDEXTENSION_VARIANT_TYPE_FLOAT &&
			api.variant_get_type(value) != GDEXTENSION_VARIANT_TYPE_INT) {
		return 0.0;
	}
	if (api.variant_get_type(value) == GDEXTENSION_VARIANT_TYPE_INT) {
		int64_t as_int = 0;
		api.to_int(&as_int, (GDExtensionVariantPtr)value);
		return (double)as_int;
	}
	api.to_float(&out, (GDExtensionVariantPtr)value);
	return out;
}

static int64_t read_int(GDExtensionConstVariantPtr value) {
	int64_t out = 0;
	if (api.variant_get_type(value) == GDEXTENSION_VARIANT_TYPE_INT) {
		api.to_int(&out, (GDExtensionVariantPtr)value);
		return out;
	}
	if (api.variant_get_type(value) == GDEXTENSION_VARIANT_TYPE_FLOAT) {
		return (int64_t)read_float(value);
	}
	return 0;
}

static GDExtensionObjectPtr create_instance(void *userdata, GDExtensionBool notify_postinitialize) {
	Mixer *mixer;
	GDExtensionObjectPtr object;
	(void)userdata;
	(void)notify_postinitialize;
	mixer = (Mixer *)api.mem_alloc(sizeof(Mixer));
	if (!mixer) {
		return 0;
	}
	mixer_init(mixer);
	object = api.classdb_construct_object3(&sn_parent);
	if (!object) {
		api.mem_free(mixer);
		return 0;
	}
	api.object_set_instance(object, &sn_class, mixer);
	return object;
}

static void free_instance(void *userdata, GDExtensionClassInstancePtr instance) {
	(void)userdata;
	if (instance) {
		api.mem_free(instance);
	}
}

static void m_transition(void *userdata, GDExtensionClassInstancePtr instance, const GDExtensionConstVariantPtr *args, GDExtensionInt arg_count, GDExtensionVariantPtr ret, GDExtensionCallError *error) {
	double params[SCENE_SCORE_PARAMS];
	int count;
	ScoreSpec spec;
	(void)userdata;
	if (!instance) {
		fail_arg(ret, error, 0, 0);
		if (error) {
			error->error = GDEXTENSION_CALL_ERROR_INSTANCE_IS_NULL;
		}
		return;
	}
	if (arg_count < 4) {
		api.variant_new_nil(ret);
		if (error) {
			error->error = GDEXTENSION_CALL_ERROR_TOO_FEW_ARGUMENTS;
			error->argument = (int32_t)arg_count;
			error->expected = 4;
		}
		return;
	}
	count = read_params(args[0], params);
	if (count < 0) {
		fail_arg(ret, error, 0, GDEXTENSION_VARIANT_TYPE_PACKED_FLOAT64_ARRAY);
		return;
	}
	score_from_params(&spec, params, count, (uint64_t)read_int(args[2]), (uint64_t)read_int(args[3]));
	mixer_transition((Mixer *)instance, &spec, read_float(args[1]));
	return_nil(ret, error);
}

static void m_adjust(void *userdata, GDExtensionClassInstancePtr instance, const GDExtensionConstVariantPtr *args, GDExtensionInt arg_count, GDExtensionVariantPtr ret, GDExtensionCallError *error) {
	double params[SCENE_SCORE_PARAMS];
	int count;
	ScoreSpec spec;
	Mixer *mixer;
	(void)userdata;
	if (!instance || arg_count < 1) {
		api.variant_new_nil(ret);
		if (error) {
			error->error = instance ? GDEXTENSION_CALL_ERROR_TOO_FEW_ARGUMENTS : GDEXTENSION_CALL_ERROR_INSTANCE_IS_NULL;
			error->argument = (int32_t)arg_count;
			error->expected = 1;
		}
		return;
	}
	count = read_params(args[0], params);
	if (count < 0) {
		fail_arg(ret, error, 0, GDEXTENSION_VARIANT_TYPE_PACKED_FLOAT64_ARRAY);
		return;
	}
	mixer = (Mixer *)instance;
	score_from_params(&spec, params, count, mixer->primary >= 0 ? mixer->layers[mixer->primary].spec.id : 0, 1);
	mixer_adjust(mixer, &spec);
	return_nil(ret, error);
}

static void m_reseed(void *userdata, GDExtensionClassInstancePtr instance, const GDExtensionConstVariantPtr *args, GDExtensionInt arg_count, GDExtensionVariantPtr ret, GDExtensionCallError *error) {
	(void)userdata;
	if (!instance || arg_count < 1) {
		api.variant_new_nil(ret);
		if (error) {
			error->error = instance ? GDEXTENSION_CALL_ERROR_TOO_FEW_ARGUMENTS : GDEXTENSION_CALL_ERROR_INSTANCE_IS_NULL;
			error->argument = (int32_t)arg_count;
			error->expected = 1;
		}
		return;
	}
	mixer_reseed((Mixer *)instance, (uint64_t)read_int(args[0]));
	return_nil(ret, error);
}

static void m_release(void *userdata, GDExtensionClassInstancePtr instance, const GDExtensionConstVariantPtr *args, GDExtensionInt arg_count, GDExtensionVariantPtr ret, GDExtensionCallError *error) {
	(void)userdata;
	(void)args;
	(void)arg_count;
	if (!instance) {
		api.variant_new_nil(ret);
		if (error) {
			error->error = GDEXTENSION_CALL_ERROR_INSTANCE_IS_NULL;
			error->argument = 0;
			error->expected = 0;
		}
		return;
	}
	mixer_release((Mixer *)instance, 0.6);
	return_nil(ret, error);
}

static void m_set_gain(void *userdata, GDExtensionClassInstancePtr instance, const GDExtensionConstVariantPtr *args, GDExtensionInt arg_count, GDExtensionVariantPtr ret, GDExtensionCallError *error) {
	(void)userdata;
	if (!instance || arg_count < 1) {
		api.variant_new_nil(ret);
		if (error) {
			error->error = instance ? GDEXTENSION_CALL_ERROR_TOO_FEW_ARGUMENTS : GDEXTENSION_CALL_ERROR_INSTANCE_IS_NULL;
			error->argument = (int32_t)arg_count;
			error->expected = 1;
		}
		return;
	}
	mixer_set_gain((Mixer *)instance, read_float(args[0]), 0.05);
	return_nil(ret, error);
}

static void m_active(void *userdata, GDExtensionClassInstancePtr instance, const GDExtensionConstVariantPtr *args, GDExtensionInt arg_count, GDExtensionVariantPtr ret, GDExtensionCallError *error) {
	(void)userdata;
	(void)args;
	(void)arg_count;
	if (!instance) {
		return_bool(ret, 0, error);
		return;
	}
	return_bool(ret, mixer_active((Mixer *)instance), error);
}

static void m_notes(void *userdata, GDExtensionClassInstancePtr instance, const GDExtensionConstVariantPtr *args, GDExtensionInt arg_count, GDExtensionVariantPtr ret, GDExtensionCallError *error) {
	(void)userdata;
	(void)args;
	(void)arg_count;
	if (!instance) {
		return_int(ret, 0, error);
		return;
	}
	return_int(ret, ((Mixer *)instance)->notes_scheduled, error);
}

static void write_frame(void *array, int64_t index, float left, float right) {
	float *slot = (float *)api.packed_vector2_array_operator_index(array, index);
	if (!slot) {
		return;
	}
	slot[0] = left;
	slot[1] = right;
}

static void m_render_into(void *userdata, GDExtensionClassInstancePtr instance, const GDExtensionConstVariantPtr *args, GDExtensionInt arg_count, GDExtensionVariantPtr ret, GDExtensionCallError *error) {
	int64_t frames;
	int64_t i;
	void *array;
	float *base;
	int contiguous = 0;
	static float scratch[8192 * 2];
	int chunk;
	(void)userdata;
	if (!instance || arg_count < 1) {
		api.variant_new_nil(ret);
		if (error) {
			error->error = instance ? GDEXTENSION_CALL_ERROR_TOO_FEW_ARGUMENTS : GDEXTENSION_CALL_ERROR_INSTANCE_IS_NULL;
			error->argument = (int32_t)arg_count;
			error->expected = 1;
		}
		return;
	}
	if (api.variant_get_type(args[0]) != GDEXTENSION_VARIANT_TYPE_PACKED_VECTOR2_ARRAY) {
		fail_arg(ret, error, 0, GDEXTENSION_VARIANT_TYPE_PACKED_VECTOR2_ARRAY);
		return;
	}
	frames = call_size(args[0]);
	if (frames < 0) {
		fail_arg(ret, error, 0, GDEXTENSION_VARIANT_TYPE_PACKED_VECTOR2_ARRAY);
		return;
	}
	if (frames == 0) {
		return_nil(ret, error);
		return;
	}
	array = api.packed_v2_ptr((GDExtensionVariantPtr)args[0]);
	base = (float *)api.packed_vector2_array_operator_index(array, 0);
	if (frames >= 2) {
		float *second = (float *)api.packed_vector2_array_operator_index(array, 1);
		contiguous = second == base + 2;
	} else {
		contiguous = 1;
	}
	i = 0;
	while (i < frames) {
		chunk = 8192;
		if ((int64_t)chunk > frames - i) {
			chunk = (int)(frames - i);
		}
		if (contiguous) {
			mixer_render((Mixer *)instance, base + i * 2, chunk);
		} else {
			int s;
			mixer_render((Mixer *)instance, scratch, chunk);
			for (s = 0; s < chunk; s++) {
				write_frame(array, i + s, scratch[s * 2], scratch[s * 2 + 1]);
			}
		}
		i += chunk;
	}
	return_nil(ret, error);
}

static void bind_method(const NameBuf *name, GDExtensionClassMethodCall call, GDExtensionVariantType ret_type, int has_ret, const GDExtensionVariantType *arg_types, const NameBuf *const *arg_names, uint32_t arg_count) {
	GDExtensionPropertyInfo args[4];
	GDExtensionClassMethodArgumentMetadata meta[4];
	GDExtensionPropertyInfo ret_info;
	GDExtensionClassMethodInfo info;
	uint32_t i;
	memset(&info, 0, sizeof(info));
	memset(args, 0, sizeof(args));
	memset(&ret_info, 0, sizeof(ret_info));
	for (i = 0; i < arg_count && i < 4; i++) {
		args[i].type = arg_types[i];
		args[i].name = (GDExtensionStringNamePtr)arg_names[i];
		args[i].class_name = &sn_empty;
		args[i].hint = 0;
		args[i].hint_string = &empty_string;
		args[i].usage = 6;
		if (arg_types[i] == GDEXTENSION_VARIANT_TYPE_INT) {
			meta[i] = GDEXTENSION_METHOD_ARGUMENT_METADATA_INT_IS_INT64;
		} else if (arg_types[i] == GDEXTENSION_VARIANT_TYPE_FLOAT) {
			meta[i] = GDEXTENSION_METHOD_ARGUMENT_METADATA_REAL_IS_DOUBLE;
		} else {
			meta[i] = GDEXTENSION_METHOD_ARGUMENT_METADATA_NONE;
		}
	}
	if (has_ret) {
		ret_info.type = ret_type;
		ret_info.name = &sn_empty;
		ret_info.class_name = &sn_empty;
		ret_info.hint_string = &empty_string;
		ret_info.usage = 6;
	}
	info.name = (GDExtensionStringNamePtr)name;
	info.method_userdata = 0;
	info.call_func = call;
	info.ptrcall_func = 0;
	info.method_flags = GDEXTENSION_METHOD_FLAGS_DEFAULT;
	info.has_return_value = has_ret ? 1 : 0;
	info.return_value_info = has_ret ? &ret_info : 0;
	info.return_value_metadata = has_ret && ret_type == GDEXTENSION_VARIANT_TYPE_INT
			? GDEXTENSION_METHOD_ARGUMENT_METADATA_INT_IS_INT64
			: GDEXTENSION_METHOD_ARGUMENT_METADATA_NONE;
	info.argument_count = arg_count;
	info.arguments_info = arg_count ? args : 0;
	info.arguments_metadata = arg_count ? meta : 0;
	info.default_argument_count = 0;
	info.default_arguments = 0;
	api.classdb_register_extension_class_method(library_ptr, &sn_class, &info);
}

static void register_class(void) {
	GDExtensionClassCreationInfo6 info;
	GDExtensionVariantType transition_types[4];
	const NameBuf *transition_names[4];
	GDExtensionVariantType one_f64[1];
	const NameBuf *one_params[1];
	GDExtensionVariantType one_int[1];
	const NameBuf *one_seed[1];
	GDExtensionVariantType one_v2[1];
	const NameBuf *one_frames[1];
	GDExtensionVariantType one_float[1];
	const NameBuf *one_gain[1];
	if (registered) {
		return;
	}
	make_name(&sn_class, "SceneScore");
	make_name(&sn_parent, "RefCounted");
	make_name(&sn_empty, "");
	make_name(&sn_size, "size");
	make_name(&sn_transition, "transition");
	make_name(&sn_render_into, "render_into");
	make_name(&sn_adjust, "adjust");
	make_name(&sn_reseed, "reseed");
	make_name(&sn_release, "release");
	make_name(&sn_set_gain, "set_gain");
	make_name(&sn_active, "active");
	make_name(&sn_notes, "notes_scheduled");
	make_name(&sn_params, "params");
	make_name(&sn_fade, "fade");
	make_name(&sn_score_id, "score_id");
	make_name(&sn_seed, "seed");
	make_name(&sn_frames, "frames");
	make_name(&sn_gain, "gain");
	make_string(&empty_string, "");
	memset(&info, 0, sizeof(info));
	info.is_exposed = 1;
	info.create_instance_func = create_instance;
	info.free_instance_func = free_instance;
	api.classdb_register_extension_class6(library_ptr, &sn_class, &sn_parent, &info);

	transition_types[0] = GDEXTENSION_VARIANT_TYPE_PACKED_FLOAT64_ARRAY;
	transition_types[1] = GDEXTENSION_VARIANT_TYPE_FLOAT;
	transition_types[2] = GDEXTENSION_VARIANT_TYPE_INT;
	transition_types[3] = GDEXTENSION_VARIANT_TYPE_INT;
	transition_names[0] = &sn_params;
	transition_names[1] = &sn_fade;
	transition_names[2] = &sn_score_id;
	transition_names[3] = &sn_seed;
	bind_method(&sn_transition, m_transition, GDEXTENSION_VARIANT_TYPE_NIL, 0, transition_types, transition_names, 4);

	one_v2[0] = GDEXTENSION_VARIANT_TYPE_PACKED_VECTOR2_ARRAY;
	one_frames[0] = &sn_frames;
	bind_method(&sn_render_into, m_render_into, GDEXTENSION_VARIANT_TYPE_NIL, 0, one_v2, one_frames, 1);

	one_f64[0] = GDEXTENSION_VARIANT_TYPE_PACKED_FLOAT64_ARRAY;
	one_params[0] = &sn_params;
	bind_method(&sn_adjust, m_adjust, GDEXTENSION_VARIANT_TYPE_NIL, 0, one_f64, one_params, 1);

	one_int[0] = GDEXTENSION_VARIANT_TYPE_INT;
	one_seed[0] = &sn_seed;
	bind_method(&sn_reseed, m_reseed, GDEXTENSION_VARIANT_TYPE_NIL, 0, one_int, one_seed, 1);
	bind_method(&sn_release, m_release, GDEXTENSION_VARIANT_TYPE_NIL, 0, 0, 0, 0);

	one_float[0] = GDEXTENSION_VARIANT_TYPE_FLOAT;
	one_gain[0] = &sn_gain;
	bind_method(&sn_set_gain, m_set_gain, GDEXTENSION_VARIANT_TYPE_NIL, 0, one_float, one_gain, 1);
	bind_method(&sn_active, m_active, GDEXTENSION_VARIANT_TYPE_BOOL, 1, 0, 0, 0);
	bind_method(&sn_notes, m_notes, GDEXTENSION_VARIANT_TYPE_INT, 1, 0, 0, 0);
	registered = 1;
}

static void unregister_class(void) {
	if (!registered) {
		return;
	}
	api.classdb_unregister_extension_class(library_ptr, &sn_class);
	registered = 0;
}

static void initialize_level(void *userdata, GDExtensionInitializationLevel level) {
	(void)userdata;
	if (level == GDEXTENSION_INITIALIZATION_SCENE) {
		register_class();
	}
}

static void deinitialize_level(void *userdata, GDExtensionInitializationLevel level) {
	(void)userdata;
	if (level == GDEXTENSION_INITIALIZATION_SCENE) {
		unregister_class();
	}
}

static int load_sym(void *dest, size_t size, GDExtensionInterfaceGetProcAddress get_proc, const char *symbol) {
	GDExtensionInterfaceFunctionPtr fn = get_proc(symbol);
	if (!fn || size != sizeof(fn)) {
		return 0;
	}
	memcpy(dest, &fn, size);
	return 1;
}

static int load_api(GDExtensionInterfaceGetProcAddress get_proc) {
	#define LOAD(field, symbol) \
		if (!load_sym(&api.field, sizeof(api.field), get_proc, symbol)) { \
			return 0; \
		}
	LOAD(get_godot_version2, "get_godot_version2");
	LOAD(mem_alloc, "mem_alloc");
	LOAD(mem_free, "mem_free");
	LOAD(print_error, "print_error");
	LOAD(classdb_register_extension_class6, "classdb_register_extension_class6");
	LOAD(classdb_unregister_extension_class, "classdb_unregister_extension_class");
	LOAD(classdb_register_extension_class_method, "classdb_register_extension_class_method");
	LOAD(classdb_construct_object3, "classdb_construct_object3");
	LOAD(object_set_instance, "object_set_instance");
	LOAD(string_name_new_with_utf8_chars, "string_name_new_with_utf8_chars");
	LOAD(string_new_with_utf8_chars, "string_new_with_utf8_chars");
	LOAD(variant_new_nil, "variant_new_nil");
	LOAD(variant_destroy, "variant_destroy");
	LOAD(variant_get_type, "variant_get_type");
	LOAD(variant_call, "variant_call");
	LOAD(get_variant_from_type_constructor, "get_variant_from_type_constructor");
	LOAD(get_variant_to_type_constructor, "get_variant_to_type_constructor");
	LOAD(variant_get_ptr_internal_getter, "variant_get_ptr_internal_getter");
	LOAD(packed_float64_array_operator_index, "packed_float64_array_operator_index");
	LOAD(packed_vector2_array_operator_index, "packed_vector2_array_operator_index");
	#undef LOAD
	api.from_bool = api.get_variant_from_type_constructor(GDEXTENSION_VARIANT_TYPE_BOOL);
	api.from_int = api.get_variant_from_type_constructor(GDEXTENSION_VARIANT_TYPE_INT);
	api.to_float = api.get_variant_to_type_constructor(GDEXTENSION_VARIANT_TYPE_FLOAT);
	api.to_int = api.get_variant_to_type_constructor(GDEXTENSION_VARIANT_TYPE_INT);
	api.packed_f64_ptr = api.variant_get_ptr_internal_getter(GDEXTENSION_VARIANT_TYPE_PACKED_FLOAT64_ARRAY);
	api.packed_v2_ptr = api.variant_get_ptr_internal_getter(GDEXTENSION_VARIANT_TYPE_PACKED_VECTOR2_ARRAY);
	if (!api.from_bool || !api.from_int || !api.to_float || !api.to_int || !api.packed_f64_ptr || !api.packed_v2_ptr) {
		return 0;
	}
	return 1;
}

SCENE_SCORE_EXPORT GDExtensionBool scene_score_library_init(GDExtensionInterfaceGetProcAddress p_get_proc_address, GDExtensionClassLibraryPtr p_library, GDExtensionInitialization *r_initialization) {
	if (!p_get_proc_address || !r_initialization || !load_api(p_get_proc_address)) {
		return 0;
	}
	library_ptr = p_library;
	r_initialization->minimum_initialization_level = GDEXTENSION_INITIALIZATION_SCENE;
	r_initialization->userdata = 0;
	r_initialization->initialize = initialize_level;
	r_initialization->deinitialize = deinitialize_level;
	return 1;
}
