package com.example.nextjsapp

import android.content.Intent
import android.os.Bundle
import androidx.activity.ComponentActivity
import androidx.activity.compose.setContent
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.material.*
import androidx.compose.runtime.Composable
import androidx.compose.runtime.rememberCoroutineScope
import androidx.compose.ui.Modifier
import com.example.nextjsapp.ui.theme.NextJsAppTheme
import kotlinx.coroutines.launch

class MainActivity : ComponentActivity() {
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContent {
            NextJsAppTheme {
                ScaffoldWithTabs()
            }
        }
    }
}

@Composable
fun ScaffoldWithTabs() {
    val scope = rememberCoroutineScope()
    val scaffoldState = rememberScaffoldState()
    var selectedTab = remember { 0 }

    Scaffold(
        scaffoldState = scaffoldState,
        topBar = {
            TopAppBar(title = { Text("My App") })
        },
        content = {
            when (selectedTab) {
                0 -> PlansScreen()
                1 -> BookReadingScreen()
            }
        },
        bottomBar = {
            BottomNavigation {
                BottomNavigationItem(
                    icon = { Icon(Icons.Default.List, contentDescription = null) },
                    label = { Text("计划表") },
                    selected = selectedTab == 0,
                    onClick = { selectedTab = 0 }
                )
                BottomNavigationItem(
                    icon = { Icon(Icons.Default.Book, contentDescription = null) },
                    label = { Text("在线看书") },
                    selected = selectedTab == 1,
                    onClick = { selectedTab = 1 }
                )
            }
        }
    )
}
