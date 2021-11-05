#pragma once

#include "instance-db.h"

#include "libplatform/libplatform.h"
#include "v8-platform.h"
#include "v8.h"
#include <unordered_map>
#include <cstring>

class v8_instance {
public:
    v8_instance(v8::Isolate::CreateParams create_params_)
    : create_params(std::move(create_params_)),
      isolate(v8::Isolate::New(create_params)) {}

    bool init_instance(const std::string& path_to_wasm_code);

    bool run_instanse(char* data);

    char* new_in_wasm(int64_t size);

    void delete_in_wasm(char* ptr);

private:
    v8::MaybeLocal<v8::String> read_file(const std::string& script_path);

    bool compile_script(const std::string script_path);

    bool create_function();

    v8::Isolate::CreateParams create_params;
    v8::Isolate* isolate{};
    v8::Global<v8::Context> context;
    v8::Global<v8::Function> function;

    std::shared_ptr<v8::BackingStore> store;
};

class v8_instance_db : public instance_db<v8_instance> {
public:
    v8_instance_db() {
        std::string flag = "--single-threaded";
        v8::V8::SetFlagsFromString(flag.c_str(), flag.size());
        platform = v8::platform::NewSingleThreadedDefaultPlatform();
        v8::V8::InitializePlatform(platform.get());
        v8::V8::Initialize();
    }

    ~v8_instance_db() {
        v8::V8::Dispose();
        v8::V8::ShutdownPlatform();
    }

private:
    bool create_instance(const std::string &wasm_path, const std::string &name) override {
        v8::Isolate::CreateParams create_params;
        create_params.array_buffer_allocator = v8::ArrayBuffer::Allocator::NewDefaultAllocator();

        auto it = all_scripts.emplace(name, std::move(create_params));
        if (!it.first->second.init_instance(wasm_path)) {
            all_scripts.erase(it.first);
            std::cout << "Can not create instance for " << name << std::endl;
            return false;
        }
        return true;
    }

    std::unique_ptr<v8::Platform> platform{};
};