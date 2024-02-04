#include <math.h>
#include <time.h>

#include "arraylist.h"
#include "init.h"

#include "clap/events.h"
#include "clap/ext/params.h"
#include "clap/ext/audio-ports.h"
#include "clap/ext/note-ports.h"
#include "clap/id.h"
#include "clap/plugin.h"
#include "clap/process.h"

static const clap_plugin_descriptor_t plugin_desc = {
    .clap_version = CLAP_VERSION_INIT,
    .id = "geff.corpobitcrusher",
    .name = "Corporate Bitcrusher",
    .vendor = "geff",
    .url = "https://geff.xyz",
    .manual_url = "https://geff.xyz",
    .support_url = "https://geff.xyz",
    .version = "1.0.0",
    .description = "yeah same",
    .features = (const char *[]){CLAP_PLUGIN_FEATURE_DISTORTION, NULL},
};

// --- clap_plugin_audio_ports_t ---

u32 countAudio(const clap_plugin_t *plugin, bool is_input) {
    return 1;
}

bool getAudio(const clap_plugin_t *plugin, u32 i, bool is_input, clap_audio_port_info_t *info) {
    if (i > 0) {
        return false;
    }

    if (is_input) {
        snprintf(info->name, sizeof(info->name), "%s", "Stereo In");
    } else {
        snprintf(info->name, sizeof(info->name), "%s", "Bitcrushed Output");
    }

    info->id = 0;
    info->channel_count = 2;
    info->flags = CLAP_AUDIO_PORT_IS_MAIN;
    info->port_type = CLAP_PORT_STEREO;
    info->in_place_pair = CLAP_INVALID_ID;
    return true;
}

static const clap_plugin_audio_ports_t extension_audio_ports = {
    .count = countAudio,
    .get = getAudio,
};

// processing helper

static void pluginProcessEvent(Plugin *plugin, const clap_event_header_t *event) {
    if (event->space_id == CLAP_CORE_EVENT_SPACE_ID) {
        switch (event->type) {
        case CLAP_EVENT_PARAM_VALUE: {
            clap_event_param_value_t *param_event = event;

            switch (param_event->param_id) {
            case PARAMS_BITDEPTH:
                plugin->bits = param_event->value;
                break;
            case PARAMS_WET:
                plugin->wet = param_event->value;
                break;
            case PARAMS_DRY:
                plugin->dry = param_event->value;
                break;
            }

            break;
        }
        }
    }
}

u32 getParamCount(const clap_plugin_t *plugin_handle) {
    return 3;
}

bool getParamInfo(const clap_plugin_t *plugin_handle, u32 param_index, clap_param_info_t *param_info) {
    switch (param_index) {
    case PARAMS_BITDEPTH:
        param_info->id = PARAMS_BITDEPTH;
        strncpy(param_info->name, "Bits", CLAP_NAME_SIZE);
        param_info->module[0] = 0;
        param_info->default_value = 16.0;
        param_info->min_value = 1.0;
        param_info->max_value = 32.0;
        param_info->flags = CLAP_PARAM_IS_AUTOMATABLE;
        param_info->cookie = NULL;
        break;
    case PARAMS_WET:
        param_info->id = PARAMS_WET;
        strncpy(param_info->name, "Wet", CLAP_NAME_SIZE);
        param_info->module[0] = 0;
        param_info->default_value = 0.8;
        param_info->min_value = 0.0;
        param_info->max_value = 1.0;
        param_info->flags = CLAP_PARAM_IS_AUTOMATABLE;
        param_info->cookie = NULL;
        break;
    case PARAMS_DRY:
        param_info->id = PARAMS_DRY;
        strncpy(param_info->name, "Dry", CLAP_NAME_SIZE);
        param_info->module[0] = 0;
        param_info->default_value = 0.3;
        param_info->min_value = 0.0;
        param_info->max_value = 1.0;
        param_info->flags = CLAP_PARAM_IS_AUTOMATABLE;
        param_info->cookie = NULL;
        break;
    default:
        return false;
    }

    return true;
}

// --- clap_plugin_params_t ---

bool getParamValue(const clap_plugin_t *plugin_handle, clap_id param_id, double *value) {
    Plugin *plugin = (Plugin *)plugin_handle->plugin_data;

    switch (param_id) {
    case PARAMS_BITDEPTH:
        *value = plugin->bits;
        return true;
    case PARAMS_WET:
        *value = plugin->wet;
        return true;
    case PARAMS_DRY:
        *value = plugin->dry;
        return true;
    }

    return false;
}

bool paramValueToText(const clap_plugin_t *plugin_handle, clap_id param_id, double value, char *display, u32 size) {
    Plugin *plugin = (Plugin *)plugin_handle->plugin_data;

    switch (param_id) {
    case PARAMS_BITDEPTH:
        snprintf(display, size, "%f bit", value);
        return true;
    case PARAMS_WET:
        snprintf(display, size, "%f", value);
        return true;
    case PARAMS_DRY:
        snprintf(display, size, "%f", value);
        return true;
    }

    return false;
}

bool paramTextToValue(const clap_plugin_t *plugin_handle, clap_id param_id, const char *display, double *value) {
    // same with this one
    return true;
}

void paramFlush(const clap_plugin_t *plugin_handle, const clap_input_events_t *in, const clap_output_events_t *out) {
    Plugin *plugin = (Plugin *)plugin_handle->plugin_data;

    for (i32 i = 0; i < in->size(in); i++) {
        const clap_event_header_t *header = in->get(in, i);
        pluginProcessEvent(plugin, header);
    }
}

static const clap_plugin_params_t params = {
    .count = getParamCount,
    .get_info = getParamInfo,
    .get_value = getParamValue,
    .value_to_text = paramValueToText,
    .text_to_value = paramTextToValue,
    .flush = paramFlush,
};

// --- clap_plugin_t ---

bool initPlugin(const struct clap_plugin *plugin_handle) {
    Plugin *plugin = (Plugin *)plugin_handle->plugin_data;

    plugin->bits = 8.0;
    plugin->wet = 0.8;
    plugin->dry = 0.3;

    return true;
}

void destroyPlugin(const struct clap_plugin *plugin_handle) {
    Plugin *plugin = (Plugin *)plugin_handle->plugin_data;
    free(plugin);
}

bool activatePlugin(const struct clap_plugin *plugin_handle, double sample_rate, u32 min_frame_count, u32 max_frame_count) {
    Plugin *plugin = (Plugin *)plugin_handle->plugin_data;
    plugin->sample_rate = sample_rate;
    return true;
}

void deactivatePlugin(const struct clap_plugin *plugin_handle) {
}

bool startProcessing(const struct clap_plugin *plugin_handle) {
    return true;
}

void stopProcessing(const struct clap_plugin *plugin_handle) {
}

void resetPlugin(const struct clap_plugin *plugin_handle) {
    Plugin *plugin = (Plugin *)plugin_handle->plugin_data;
}

static clap_process_status process(const struct clap_plugin *plugin_handle, const clap_process_t *process_clap) {
    Plugin *plugin = (Plugin *)plugin_handle->plugin_data;

    u32 event_index = 0;
    u32 next_event_frame = process_clap->in_events->size(process_clap->in_events) > 0 ? 0 : process_clap->frames_count;

    for (u32 i = 0; i < process_clap->frames_count;) {
        // handle all events
        while (event_index < process_clap->in_events->size(process_clap->in_events) && next_event_frame == i) {
            const clap_event_header_t *header = process_clap->in_events->get(process_clap->in_events, event_index);

            if (header->time != i) {
                next_event_frame = header->time;
                break;
            }

            pluginProcessEvent(plugin, header);
            ++event_index;

            if (event_index == process_clap->in_events->size(process_clap->in_events)) {
                next_event_frame = process_clap->frames_count;
                break;
            }
        }

        for (; i < next_event_frame; i++) {
            float in_l = process_clap->audio_inputs[0].data32[0][i];
            float in_r = process_clap->audio_inputs[0].data32[1][i];

            float wet_l, wet_r, dry_l = in_l, dry_r = in_r;

            dry_l *= plugin->dry;
            dry_r *= plugin->dry;
            wet_l = (floor(in_l * pow(2.0, plugin->bits)) / pow(2.0, plugin->bits)) * plugin->wet;
            wet_r = (floor(in_r * pow(2.0, plugin->bits)) / pow(2.0, plugin->bits)) * plugin->wet;

            process_clap->audio_outputs[0].data32[0][i] = wet_l + dry_l;
            process_clap->audio_outputs[0].data32[1][i] = wet_r + dry_r;
        }
    }

    return CLAP_PROCESS_CONTINUE;
}

const void *getExtension(const struct clap_plugin *plugin_handle, const char *id) {
    if (strcmp(id, CLAP_EXT_AUDIO_PORTS) == 0) {
        return &extension_audio_ports;
    }
    if (strcmp(id, CLAP_EXT_PARAMS) == 0) {
        return &params;
    }

    return NULL;
}

void onMainThread(const struct clap_plugin *plugin_handle) {
}

static const clap_plugin_t plugin_class = {
    .desc = &plugin_desc,
    .plugin_data = NULL,
    .init = initPlugin,
    .destroy = destroyPlugin,
    .activate = activatePlugin,
    .deactivate = deactivatePlugin,
    .start_processing = startProcessing,
    .stop_processing = stopProcessing,
    .reset = resetPlugin,
    .process = process,
    .get_extension = getExtension,
    .on_main_thread = onMainThread,
};

// --- clap_plugin_factory_t ---

uint32_t getPluginCount(const struct clap_plugin_factory *factory) {
    return 1;
}

const clap_plugin_descriptor_t *getPluginDescriptor(const struct clap_plugin_factory *factory, uint32_t index) {
    return &plugin_desc;
}

const clap_plugin_t *createPlugin(const struct clap_plugin_factory *factory, const clap_host_t *host, const char *plugin_id) {
    if (!clap_version_is_compatible(host->clap_version)) {
        return NULL;
    }

    Plugin *plugin = (Plugin *)calloc(1, sizeof(Plugin));
    plugin->host = host;
    plugin->plugin = plugin_class;
    plugin->plugin.plugin_data = plugin;
    return &plugin->plugin;
}

static const clap_plugin_factory_t plugin_factory = {
    .get_plugin_count = getPluginCount,
    .get_plugin_descriptor = getPluginDescriptor,
    .create_plugin = createPlugin,
};

// --- clap_plugin_entry_t ---

bool init(const char *path) {
    return true;
}

void deinit() {
}

const void *getFactory(const char *factory_id) {
    return strcmp(factory_id, CLAP_PLUGIN_FACTORY_ID) ? NULL : &plugin_factory;
}

const clap_plugin_entry_t clap_entry = {
    .clap_version = CLAP_VERSION_INIT,
    .init = init,
    .deinit = deinit,
    .get_factory = getFactory,
};
