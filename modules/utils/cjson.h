// MODULES / UTILS / CJSON.H

#ifndef UTILS_CJSON_H
  #define UTILS_CJSON_H

  static int _utils_cjson_is_json_object(cJSON *item);
  void       _utils_cjson_merge_objects(cJSON *destination, cJSON *source);
  
#endif
