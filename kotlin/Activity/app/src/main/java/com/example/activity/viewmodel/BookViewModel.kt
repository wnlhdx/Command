package com.example.activity.viewmodel

import androidx.lifecycle.ViewModel
import androidx.lifecycle.viewModelScope
import com.example.activity.repository.BookRepository
import kotlinx.coroutines.flow.MutableStateFlow
import kotlinx.coroutines.flow.StateFlow
import kotlinx.coroutines.launch

class BookViewModel : ViewModel() {
    private val bookRepository = BookRepository()

    private val _currentPage = MutableStateFlow(1)
    val currentPage: StateFlow<Int> get() = _currentPage

    private val _bookPic = MutableStateFlow("")
    val bookPic: StateFlow<String> get() = _bookPic

    private val _bookTranslation = MutableStateFlow("")
    val bookTranslation: StateFlow<String> get() = _bookTranslation

    init {
        fetchBookInfo(_currentPage.value)
    }

    fun previousPage() {
        if (_currentPage.value > 1) {
            _currentPage.value -= 1
            fetchBookInfo(_currentPage.value)
        }
    }

    fun nextPage() {
        _currentPage.value += 1
        fetchBookInfo(_currentPage.value)
    }

    private fun fetchBookInfo(page: Int) {
        viewModelScope.launch {
            _bookPic.value = bookRepository.getBookPic(page)
            _bookTranslation.value = bookRepository.getBookTranslation(page)
        }
    }
}
