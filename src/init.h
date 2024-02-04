#ifndef INIT_H
#define INIT_H

#include "Utility.h"

#include "arraylist.h"

#include "clap/clap.h"
#include "clap/factory/plugin-factory.h"
#include "clap/plugin-features.h"
#include "clap/plugin.h"

typedef struct {
    clap_plugin_t plugin;
    const clap_host_t *host;
    float sample_rate;
    double bits;
    double wet, dry;
} Plugin;

typedef enum {
    BitDepth,
    Wet,
    Dry,
} Params;

#endif