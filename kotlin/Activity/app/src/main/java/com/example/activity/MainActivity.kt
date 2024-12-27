import android.os.Bundle
import androidx.activity.ComponentActivity
import androidx.activity.compose.setContent
import androidx.compose.foundation.layout.*
import androidx.compose.foundation.lazy.LazyColumn
import androidx.compose.foundation.lazy.items
import androidx.compose.material3.*
import androidx.compose.runtime.*
import androidx.compose.ui.Modifier
import androidx.compose.ui.unit.dp
import com.example.activity.ui.theme.ActivityTheme
import androidx.compose.material.icons.Icons
import androidx.compose.material.icons.filled.*


class MainActivity : ComponentActivity() {
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContent {
            ActivityTheme {
                PlanScreen()
            }
        }
    }
}

@OptIn(ExperimentalMaterial3Api::class)
@Composable
fun PlanScreen() {
    var plans by remember { mutableStateOf<List<Plan>>(emptyList()) }
    var isEditing by remember { mutableStateOf(false) }
    var currentPlan by remember { mutableStateOf<Plan?>(null) }

    // 获取计划
    LaunchedEffect(Unit) {
        fetchPlans { fetchedPlans ->
            plans = fetchedPlans
        }
    }

    Scaffold(
        topBar = {
            TopAppBar(title = { Text("计划管理") })
        },
        content = { padding ->
            Column(
                modifier = Modifier
                    .fillMaxSize()
                    .padding(padding)
            ) {
                Button(
                    onClick = {
                        isEditing = false
                        currentPlan = Plan(
                            "",
                            "",
                            "",
                            "",
                            "",
                            "",
                            "",
                            "",
                            "",
                            "",
                            "",
                            "",
                            "",
                            "",
                            "",
                            "",
                            "",
                            "",
                            "",
                            "",
                            "",
                             ""
                        )
                    },
                    modifier = Modifier.padding(16.dp)
                ) {
                    Text("添加计划")
                }

                LazyColumn {
                    items(plans) { plan ->
                        PlanRow(
                            plan = plan,
                            onEdit = {
                                isEditing = true
                                currentPlan = plan
                            },
                            onDelete = {
                                deletePlan(plan.planName) {
                                    fetchPlans { fetchedPlans -> plans = fetchedPlans }
                                }
                            }
                        )
                    }
                }

                // 弹出对话框，显示计划信息并编辑
                if (currentPlan != null) {
                    PlanDialog(
                        plan = currentPlan!!,
                        isEditing = isEditing,
                        onSave = { plan ->
                            savePlan(plan, isEditing) {
                                fetchPlans { fetchedPlans -> plans = fetchedPlans }
                            }
                        }
                    )
                }
            }
        }
    )
}

@Composable
fun PlanRow(plan: Plan, onEdit: () -> Unit, onDelete: () -> Unit) {
    Row(
        modifier = Modifier.fillMaxWidth().padding(16.dp),
        horizontalArrangement = Arrangement.SpaceBetween
    ) {
        Text(plan.planName)
        Row {
            IconButton(onClick = onEdit) {
                Icon(Icons.Default.Edit, contentDescription = "编辑")
            }
            IconButton(onClick = onDelete) {
                Icon(Icons.Default.Delete, contentDescription = "删除")
            }
        }
    }
}

@Composable
fun PlanDialog(plan: Plan, isEditing: Boolean, onSave: (Plan) -> Unit) {
    var name by remember { mutableStateOf(plan.planName) }

    AlertDialog(
        onDismissRequest = { },
        title = { Text(if (isEditing) "编辑计划" else "添加计划") },
        text = {
            Column {
                TextField(value = name, onValueChange = { name = it })
                // 可以添加更多的字段
            }
        },
        confirmButton = {
            TextButton(onClick = {
                onSave(plan.copy(planName = name))
            }) {
                Text("保存")
            }
        },
        dismissButton = {
            TextButton(onClick = { /* 关闭对话框 */ }) {
                Text("取消")
            }
        }
    )
}
