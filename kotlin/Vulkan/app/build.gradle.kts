// app 模块中的 build.gradle.kts

plugins {
    alias(libs.plugins.android.application)
    alias(libs.plugins.kotlin.android)
}

android {
    namespace = "com.example.vulkan"
    compileSdk = 34

    defaultConfig {
        applicationId = "com.example.vulkan"
        minSdk = 34
        targetSdk = 34
        versionCode = 1
        versionName = "1.0"
        ndkVersion = "27.1.12297006"  // 指定 NDK 版本

        testInstrumentationRunner = "androidx.test.runner.AndroidJUnitRunner"

        // 配置外部原生构建（使用 CMake）
        externalNativeBuild {
            cmake {
                cppFlags += "-std=c++20 -DVULKAN_ENABLED"
            }
        }

        // 配置 NDK，支持的 ABI 过滤器
        ndk {
            abiFilters += "arm64-v8a"  // 增加支持的 ABI 架构
        }
    }

    // 取消掉的 sourceSets 配置（如果需要配置 JNI 库路径，可以放开）
    /*
    sourceSets {
        getByName("main") {
            jniLibs.srcDirs(files("src/main/jniLibs"))  // 如果有 JNI 库需要指定路径
        }
    }
    */

    buildTypes {
        release {
            isMinifyEnabled = false  // 关闭代码压缩
            proguardFiles(
                getDefaultProguardFile("proguard-android-optimize.txt"),
                "proguard-rules.pro"  // 使用自定义的 ProGuard 配置
            )
        }
    }

    // Java 编译选项
    compileOptions {
        sourceCompatibility = JavaVersion.VERSION_1_8
        targetCompatibility = JavaVersion.VERSION_1_8
    }

    // Kotlin 编译选项
    kotlinOptions {
        jvmTarget = "1.8"
    }

    // 配置外部原生构建（CMake）
    externalNativeBuild {
        cmake {
            path = file("src/main/cpp/CMakeLists.txt")  // CMakeLists.txt 的路径
            version = "3.31.2"  // CMake 版本
        }
    }

    // 开启 ViewBinding
    buildFeatures {
        viewBinding = true
    }
}

dependencies {

    implementation(libs.androidx.core.ktx)
    implementation(libs.androidx.appcompat)
    implementation(libs.material)
    implementation(libs.androidx.games.activity)
    testImplementation(libs.junit)
    androidTestImplementation(libs.androidx.junit)
    androidTestImplementation(libs.androidx.espresso.core)
}
