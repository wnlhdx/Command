plugins {
    alias(libs.plugins.android.application)
    alias(libs.plugins.jetbrains.kotlin.android)
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
        ndkVersion = "27.1.12297006"
        testInstrumentationRunner = "androidx.test.runner.AndroidJUnitRunner"
        externalNativeBuild {
            cmake {
                cppFlags += "-std=c++20 -DVULKAN_ENABLED"
            }
        }
        ndk {
            abiFilters += "arm64-v8a"  // 使用 `+=` 添加新的 ABI，而不是重新赋值
        }
    }

    //sourceSets {
    //    getByName("main") {
            // 这里改用 files() 方法来设置路径
    //        jniLibs.srcDirs(files("src/main/jniLibs"))
    //    }
    //}

    buildTypes {
        release {
            isMinifyEnabled = false
            proguardFiles(
                getDefaultProguardFile("proguard-android-optimize.txt"),
                "proguard-rules.pro"
            )
        }
    }

    compileOptions {
        sourceCompatibility = JavaVersion.VERSION_1_8
        targetCompatibility = JavaVersion.VERSION_1_8
    }

    kotlinOptions {
        jvmTarget = "1.8"
    }

    externalNativeBuild {
        cmake {
            path = file("src/main/cpp/CMakeLists.txt")
            version = "3.31.2"
        }
    }

    buildFeatures {
        viewBinding = true
    }
}

dependencies {
    implementation(libs.androidx.core.ktx)
    implementation(libs.androidx.appcompat)
    implementation(libs.material)
    implementation(libs.androidx.constraintlayout)
    testImplementation(libs.junit)
    androidTestImplementation(libs.androidx.junit)
    androidTestImplementation(libs.androidx.espresso.core)
}
