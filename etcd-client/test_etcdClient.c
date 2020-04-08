//
// Created by chugang on 2020/4/7.
//

#include "etcd-client/etcdClient.h"
#include "utils/cJSON/cJSON.h"

void test_CreatePod();

void test_getPod();

void test_cjson();

char *create_monitor(void);

int supports_full_hd(const char *const monitor);

int parsePod();

int main() {
//    test_CreatePod();
//    test_getPod();
//    test_cjson();
//    char *str = create_monitor();
//    supports_full_hd(str);
    parsePod();
}

void test_CreatePod() {
    Pod pod;
    pod.id = "nginx2";
    pod.config = "{\n"
                 "  \"kind\": \"Pod\",\n"
                 "  \"apiVersion\": \"v1beta1\",\n"
                 "  \"id\": \"php\",\n"
                 "  \"desiredState\": {\n"
                 "    \"manifest\": {\n"
                 "      \"version\": \"v1beta1\",\n"
                 "      \"id\": \"php\",\n"
                 "      \"containers\": [{\n"
                 "        \"name\": \"nginx\",\n"
                 "        \"image\": \"nginx\",\n"
                 "        \"ports\": [{\n"
                 "          \"containerPort\": 80,\n"
                 "          \"hostPort\": 8082\n"
                 "        }],\n"
                 "        \"livenessProbe\": {\n"
                 "          \"enabled\": true,\n"
                 "          \"type\": \"http\",\n"
                 "          \"initialDelaySeconds\": 30,\n"
                 "          \"httpGet\": {\n"
                 "            \"path\": \"/index.html\",\n"
                 "            \"port\": \"8082\"\n"
                 "          }\n"
                 "        }\n"
                 "      }]\n"
                 "    }\n"
                 "  },\n"
                 "  \"labels\": {\n"
                 "    \"name\": \"foo\"\n"
                 "  }\n"
                 "}\n"
                 "";
//    pod.config = "hello23-nginx";
    char *body = (char *) malloc(sizeof(char) * BUFSIZE * 4);
    createPod(pod, body);
    printf("body:\n%s", body);
}

void test_getPod() {
    char podId[7] = "nginx2";
    char *body = (char *) malloc(sizeof(char) * BUFSIZE * 4);
    printf("===========test\n");
    getPod(podId, body);
    printf("body:\n%s", body);
}

//create a monitor with a list of supported resolutions
//NOTE: Returns a heap allocated string, you are required to free it after use.
char *create_monitor(void) {
    const unsigned int resolution_numbers[3][2] = {
            {1280, 720},
            {1920, 1080},
            {3840, 2160}
    };
    char *string = NULL;
    cJSON *name = NULL;
    cJSON *resolutions = NULL;
    cJSON *resolution = NULL;
    cJSON *width = NULL;
    cJSON *height = NULL;
    size_t index = 0;

    cJSON *monitor = cJSON_CreateObject();
    if (monitor == NULL) {
        goto end;
    }

    name = cJSON_CreateString("Awesome 4K");
    if (name == NULL) {
        goto end;
    }
    /* after creation was successful, immediately add it to the monitor,
     * thereby transferring ownership of the pointer to it */
    cJSON_AddItemToObject(monitor, "name", name);

    resolutions = cJSON_CreateArray();
    if (resolutions == NULL) {
        goto end;
    }
    cJSON_AddItemToObject(monitor, "resolutions", resolutions);

    for (index = 0; index < (sizeof(resolution_numbers) / (2 * sizeof(int))); ++index) {
        resolution = cJSON_CreateObject();
        if (resolution == NULL) {
            goto end;
        }
        cJSON_AddItemToArray(resolutions, resolution);

        width = cJSON_CreateNumber(resolution_numbers[index][0]);
        if (width == NULL) {
            goto end;
        }
        cJSON_AddItemToObject(resolution, "width", width);

        height = cJSON_CreateNumber(resolution_numbers[index][1]);
        if (height == NULL) {
            goto end;
        }
        cJSON_AddItemToObject(resolution, "height", height);
    }

    string = cJSON_Print(monitor);
    if (string == NULL) {
        fprintf(stderr, "Failed to print monitor.\n");
    }

    end:
    cJSON_Delete(monitor);
    return string;
}

/* return 1 if the monitor supports full hd, 0 otherwise */
int supports_full_hd(const char *const monitor) {
    const cJSON *resolution = NULL;
    const cJSON *resolutions = NULL;
    const cJSON *name = NULL;
    int status = 0;
    cJSON *monitor_json = cJSON_Parse(monitor);
    if (monitor_json == NULL) {
        const char *error_ptr = cJSON_GetErrorPtr();
        if (error_ptr != NULL) {
            fprintf(stderr, "Error before: %s\n", error_ptr);
        }
        status = 0;
        goto end;
    }

    name = cJSON_GetObjectItemCaseSensitive(monitor_json, "name");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        printf("Checking monitor \"%s\"\n", name->valuestring);
    }

    resolutions = cJSON_GetObjectItemCaseSensitive(monitor_json, "resolutions");
    cJSON_ArrayForEach(resolution, resolutions) {
        cJSON *width = cJSON_GetObjectItemCaseSensitive(resolution, "width");
        cJSON *height = cJSON_GetObjectItemCaseSensitive(resolution, "height");
        printf("width = %d\nheight = %d\n", width->valueint, height->valueint);

        if (!cJSON_IsNumber(width) || !cJSON_IsNumber(height)) {
            status = 0;
            goto end;
        }

        if ((width->valuedouble == 1920) && (height->valuedouble == 1080)) {
            status = 1;
            goto end;
        }
    }

    end:
    cJSON_Delete(monitor_json);
    return status;
}

void test_cjson() {
    FILE *fp = NULL;
    cJSON *json;
    char *out;
    char line[1024] = {0};

    if (NULL != (fp = fopen("/Users/cg/data/code/wheel/c/c-k8s/etcd-client/test.ply", "r"))) {
        while (NULL != fgets(line, sizeof(line), fp)) {
            json = cJSON_Parse(line); //获取整个大的句柄
            out = cJSON_Print(json);  //这个是可以输出的。为获取的整个json的值
            cJSON *arrayItem = cJSON_GetObjectItem(json, "syslog_db"); //获取这个对象成员
            cJSON *object = cJSON_GetArrayItem(arrayItem, 0);   //因为这个对象是个数组获取，且只有一个元素所以写下标为0获取

            /*下面就是可以重复使用cJSON_GetObjectItem来获取每个成员的值了*/
            cJSON *item = cJSON_GetObjectItem(object, "db_user");  //
            printf("db_user:%s\n", item->valuestring);

            item = cJSON_GetObjectItem(object, "db_password");
            printf("db_password:%s\n", item->valuestring);

            item = cJSON_GetObjectItem(object, "db_type");
            printf("db_type:%s\n", item->valuestring);

            item = cJSON_GetObjectItem(object, "db_ip");
            printf("db_ip:%s\n", item->valuestring);

            item = cJSON_GetObjectItem(object, "db_port");
            printf("db_port:%s\n", item->valuestring);

            item = cJSON_GetObjectItem(object, "db_name");
            printf("db_name:%s\n", item->valuestring);

            item = cJSON_GetObjectItem(object, "sql");
            printf("db_sql:%s\n", item->valuestring);


            /*这里这个是直接可以获取值的*/
            arrayItem = cJSON_GetObjectItem(json, "syslog_enable");
            printf("%s\n", arrayItem->valuestring);
        }
    }
    cJSON_Delete(json);
//    close(fp);
}

int parsePod() {
    char podId[7] = "nginx2";
    char *body = (char *) malloc(sizeof(char) * BUFSIZE * 4);
    getPod(podId, body);
    printf("===========body\n%s", body);
    const cJSON *action = NULL;
    const cJSON *node = NULL;
    const cJSON *desiredState = NULL;
    const cJSON *manifest = NULL;

    const cJSON *key = NULL;

    int status = 0;
    cJSON *monitor_json = cJSON_Parse(body);
    if (monitor_json == NULL) {
        const char *error_ptr = cJSON_GetErrorPtr();
        if (error_ptr != NULL) {
            fprintf(stderr, "Error before: %s\n", error_ptr);
        }
        status = 0;
    }

    action = cJSON_GetObjectItemCaseSensitive(monitor_json, "action");
    if (cJSON_IsString(action) && (action->valuestring != NULL)) {
        printf("Checking monitor \"%s\"\n", action->valuestring);
    }

    node = cJSON_GetObjectItemCaseSensitive(monitor_json, "node");
    printf("node = %s\n", node->string);
    key = cJSON_GetObjectItem(node, "key");
    printf("key = %s\n", key->valuestring);
    cJSON *value = cJSON_GetObjectItem(node, "value");
    printf("value = %s\n", value->valuestring);

    cJSON *valueJSON = cJSON_Parse(value->valuestring);
    cJSON *kind = cJSON_GetObjectItemCaseSensitive(valueJSON, "kind");
    printf("kind = %s\n", kind->valuestring);
//
//    if (cJSON_IsString(node) && (node->valuestring != NULL)) {
//        printf("Checking monitor \"%s\"\n", node->valuestring);
//    }
//
//    desiredState = cJSON_GetObjectItemCaseSensitive(monitor_json, "desiredState");
//    if (cJSON_IsString(desiredState) && (desiredState->valuestring != NULL)) {
//        printf("Checking monitor \"%s\"\n", desiredState->valuestring);
//    }

    return 1;
}