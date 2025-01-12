package  com.example.activity;

import android.os.Bundle
import androidx.activity.ComponentActivity
import androidx.activity.compose.setContent
import androidx.compose.foundation.background
import androidx.compose.foundation.layout.*
import androidx.compose.material.icons.Icons
import androidx.compose.material.icons.automirrored.filled.List
import androidx.compose.material.icons.filled.Book
import androidx.compose.material3.*
import androidx.compose.runtime.*
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.unit.dp


@Composable
fun CustomTopAppBar(title: String) {
    // 使用 Row 和 Text 创建自定义顶部应用栏
    Row(
        modifier = Modifier
            .fillMaxWidth()
            .padding(16.dp)
            .height(56.dp)
            .background(MaterialTheme.colorScheme.primary)
            .padding(horizontal = 16.dp),
        verticalAlignment = Alignment.CenterVertically
    ) {
        Text(
            text = title,
            style = MaterialTheme.typography.titleLarge.copy(
                color = Color.White,
                fontWeight = FontWeight.Bold
            )
        )
    }
}


@Composable
fun ScaffoldWithTabs() {
    var selectedTab by remember { mutableStateOf(0) }

    Scaffold(
        topBar = {
            CustomTopAppBar(title = "在线管理与阅读")
        },
        content = { innerPadding ->
            // 使用 padding 来正确处理内边距
            Column(modifier = Modifier.padding(innerPadding)) {
                when (selectedTab) {
                    0 -> PlansScreen()
                    1 -> BookReadingScreen()
                }
            }
        },
        bottomBar = {
            NavigationBar  {
                NavigationBarItem(
                    icon = { Icon(Icons.AutoMirrored.Filled.List, contentDescription = "计划表") },
                    label = { Text("计划表") },
                    selected = selectedTab == 0,
                    onClick = { selectedTab = 0 }
                )
                NavigationBarItem(
                    icon = { Icon(Icons.Default.Book, contentDescription = "在线看书") },
                    label = { Text("在线看书") },
                    selected = selectedTab == 1,
                    onClick = { selectedTab = 1 }
                )
            }
        }
    )
}

class MainActivity : ComponentActivity() {
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContent {
            MaterialTheme {
                ScaffoldWithTabs()
            }
        }
    }
}
