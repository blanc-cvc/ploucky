// MODULES / UTILS / CJSON.C

#include "../_libs/cjson/cJSON.h"

static int _utils_cjson_is_json_object(cJSON *item) {
    return (item != NULL && cJSON_IsObject(item));
}

void _utils_cjson_merge_objects(cJSON *destination, cJSON *source) {
    if (!_utils_cjson_is_json_object(destination) || !_utils_cjson_is_json_object(source)) {
        return;
    }

    cJSON *item;
    cJSON_ArrayForEach(item, source) {
        cJSON *existing = cJSON_GetObjectItemCaseSensitive(destination, item->string);

        if (_utils_cjson_is_json_object(existing) && _utils_cjson_is_json_object(item)) {
            _utils_cjson_merge_objects(existing, item);
        } else {
            cJSON *copy = cJSON_Duplicate(item, 1);
            cJSON_AddItemToObjectCS(destination, item->string, copy);
        }
    }
}
