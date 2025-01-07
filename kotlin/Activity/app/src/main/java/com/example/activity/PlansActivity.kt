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
                    onDelete = { viewModel.deletePlan(it) }
                )
            }
        }
    }

    // 弹窗：新增或编辑计划
    if (isEditing) {
        PlanEditDialog(
            initialPlan = selectedPlan,
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
