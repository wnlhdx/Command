package  com.example.activity;

import androidx.compose.foundation.layout.*
import androidx.compose.foundation.lazy.LazyColumn
import androidx.compose.foundation.lazy.items
import androidx.compose.material.icons.Icons
import androidx.compose.material.icons.filled.Delete
import androidx.compose.material.icons.filled.Edit
import androidx.compose.material3.*
import androidx.compose.runtime.*
import androidx.compose.ui.Modifier
import androidx.compose.ui.unit.dp
import androidx.lifecycle.viewmodel.compose.viewModel
import com.example.activity.model.Plan
import com.example.activity.viewmodel.PlanViewModel

@Composable
fun PlansScreen(viewModel: PlanViewModel = viewModel()) {
    val plans by viewModel.plans.collectAsState(initial = emptyList())
    val (isEditing, setEditing) = remember { mutableStateOf(false) }
    val (selectedPlan, setSelectedPlan) = remember { mutableStateOf<Plan?>(null) }

    Column(modifier = Modifier.fillMaxSize()) {
        // 新增计划按钮
        Button(
            onClick = {
                setSelectedPlan(null) // 创建新计划
                setEditing(true)
            },
            modifier = Modifier.padding(16.dp)
        ) {
            Text("新增计划")
        }

        // 计划列表
        LazyColumn {
            items(plans) { plan ->
                PlanItem(
                    plan = plan,
                    onEdit = {
                        setSelectedPlan(it)
                        setEditing(true)
                    },
                    onDelete = {
                        viewModel.deletePlan(plan) // 调用 ViewModel 删除计划
                    }
                )
            }
        }
    }

    // 弹窗：新增或编辑计划
    if (isEditing) {
        PlanDialog(
            plan = selectedPlan,
            onDismiss = { setEditing(false) },
            onSave = { newPlan ->
                if (selectedPlan == null) {
                    viewModel.createPlan(newPlan) // 新增计划
                } else {
                    viewModel.updatePlan(newPlan) // 修改计划
                }
                setEditing(false)
            }
        )
    }
}

@Composable
fun PlanItem(plan: Plan, onEdit: (Plan) -> Unit, onDelete: (Plan) -> Unit) {
    Card(modifier = Modifier.padding(8.dp)) {
        Column(modifier = Modifier.padding(16.dp)) {
            // 显示 Plan 中的所有字段
            Text(text = "计划名: ${plan.planName}", style = MaterialTheme.typography.bodyLarge)
            Text(text = "项目名: ${plan.projectName}", style = MaterialTheme.typography.bodyMedium)
            Text(text = "计划详情: ${plan.planDetails}", style = MaterialTheme.typography.bodyMedium)
            Text(text = "项目详情: ${plan.projectDetails}", style = MaterialTheme.typography.bodyMedium)
            Text(text = "完成百分比: ${plan.projectFinishPercent}", style = MaterialTheme.typography.bodyMedium)
            Text(text = "学习方式: ${plan.typeOfLearn}", style = MaterialTheme.typography.bodyMedium)
            Text(text = "书籍: ${plan.bookName}", style = MaterialTheme.typography.bodyMedium)
            Text(text = "书籍内容: ${plan.bookContent}", style = MaterialTheme.typography.bodyMedium)
            Text(text = "学科: ${plan.majorIn}", style = MaterialTheme.typography.bodyMedium)
            Text(text = "项目时间: ${plan.projectMonth} ${plan.projectYear}", style = MaterialTheme.typography.bodyMedium)
            Text(text = "休闲项目: ${plan.relaxItem}", style = MaterialTheme.typography.bodyMedium)
            Text(text = "学习类型详情: ${plan.typeDetail}", style = MaterialTheme.typography.bodyMedium)

            // 你可以继续增加更多字段的显示

            // 编辑和删除按钮
            Row(modifier = Modifier.fillMaxWidth(), horizontalArrangement = Arrangement.End) {
                IconButton(onClick = { onEdit(plan) }) {
                    Icon(imageVector = Icons.Default.Edit, contentDescription = "编辑")
                }
                IconButton(onClick = { onDelete(plan) }) {
                    Icon(imageVector = Icons.Default.Delete, contentDescription = "删除")
                }
            }
        }
    }
}
