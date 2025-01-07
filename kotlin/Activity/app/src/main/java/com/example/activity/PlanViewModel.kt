package com.example.nextjsapp.viewmodel

import androidx.lifecycle.ViewModel
import androidx.lifecycle.viewModelScope
import com.example.nextjsapp.model.Plan
import com.example.nextjsapp.repository.PlanRepository
import kotlinx.coroutines.flow.MutableStateFlow
import kotlinx.coroutines.flow.StateFlow
import kotlinx.coroutines.launch

class PlanViewModel : ViewModel() {
    private val planRepository = PlanRepository()

    private val _plans = MutableStateFlow<List<Plan>>(emptyList())
    val plans: StateFlow<List<Plan>> get() = _plans

    init {
        fetchAllPlans()
    }

    private fun fetchAllPlans() {
        viewModelScope.launch {
            _plans.value = planRepository.getAllPlans()
        }
    }
}
