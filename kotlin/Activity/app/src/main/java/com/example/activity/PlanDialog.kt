@Composable
fun PlanDialog(plan: Plan?, onDismiss: () -> Unit, onSave: (Plan) -> Unit) {
    val planName = remember { mutableStateOf(plan?.planName ?: "") }
    val planDetails = remember { mutableStateOf(plan?.planDetails ?: "") }
    val projectName = remember { mutableStateOf(plan?.projectName ?: "") }
    val projectDetails = remember { mutableStateOf(plan?.projectDetails ?: "") }

    AlertDialog(
        onDismissRequest = { onDismiss() },
        title = { Text(text = if (plan == null) "新增计划" else "编辑计划") },
        text = {
            Column {
                TextField(
                    value = planName.value,
                    onValueChange = { planName.value = it },
                    label = { Text("计划名") }
                )
                TextField(
                    value = planDetails.value,
                    onValueChange = { planDetails.value = it },
                    label = { Text("计划详情") }
                )
                TextField(
                    value = projectName.value,
                    onValueChange = { projectName.value = it },
                    label = { Text("项目名") }
                )
                TextField(
                    value = projectDetails.value,
                    onValueChange = { projectDetails.value = it },
                    label = { Text("项目详情") }
                )
            }
        },
        confirmButton = {
            Button(onClick = {
                onSave(
                    Plan(
                        planName = planName.value,
                        planDetails = planDetails.value,
                        projectName = projectName.value,
                        projectDetails = projectDetails.value,
                        // 以下字段可添加默认值或根据实际需求调整
                        timeStart = plan?.timeStart ?: Date(),
                        timeEnd = plan?.timeEnd ?: Date(),
                        bookName = plan?.bookName ?: "",
                        bookContent = plan?.bookContent ?: "",
                        projectFinishPercent = plan?.projectFinishPercent ?: "0%",
                        majorIn = plan?.majorIn ?: "",
                        projectMonth = plan?.projectMonth ?: "",
                        projectYear = plan?.projectYear ?: "",
                        relaxItem = plan?.relaxItem ?: "",
                        relaxItemForeign = plan?.relaxItemForeign ?: "",
                        typeOfLearn = plan?.typeOfLearn ?: "",
                        typeDetail = plan?.typeDetail ?: "",
                        standardLearn = plan?.standardLearn ?: "",
                        updateTime = plan?.updateTime ?: Date(),
                        bookPage = plan?.bookPage ?: ""
                    )
                )
            }) {
                Text("保存")
            }
        },
        dismissButton = {
            Button(onClick = { onDismiss() }) {
                Text("取消")
            }
        }
    )
}
