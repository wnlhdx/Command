package com.example.nextjsapp.repository

import com.example.nextjsapp.network.RetrofitClient

class BookRepository {
    private val api = RetrofitClient.apiService

    suspend fun getBookPic(page: Int): String {
        return api.getBookPic(page)
    }

    suspend fun getBookTranslation(page: Int): String {
        return api.getBookTranslation(page)
    }

    suspend fun getBookPage(): Int {
        return api.getBookPage()
    }

    suspend fun getBookName(): String {
        return api.getBookName()
    }
}
