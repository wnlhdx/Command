package com.example.activity.viewmodel

import androidx.lifecycle.ViewModel
import androidx.lifecycle.viewModelScope
import com.example.activity.model.Plan
import com.example.activity.repository.PlanRepository
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

    // 新增计划
    fun createPlan(plan: Plan) {
        viewModelScope.launch {
            val newPlan = planRepository.createPlan(plan)
            _plans.value += newPlan // 更新计划列表
        }
    }

    // 修改计划
    fun updatePlan(plan: Plan) {
        viewModelScope.launch {
            val updatedPlan = planRepository.updatePlan(plan.planName, plan)
            _plans.value = _plans.value.map {
                if (it.planName == plan.planName) updatedPlan else it
            } // 更新计划列表
        }
    }

    // 删除计划
    fun deletePlan(plan: Plan) {
        viewModelScope.launch {
            planRepository.deletePlan(plan.planName)
            _plans.value = _plans.value.filterNot { it.planName == plan.planName } // 删除计划
        }
    }
}

