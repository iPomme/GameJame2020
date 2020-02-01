#if UNITY_EDITOR
using System.Linq;
using DefaultNamespace;
using UnityEngine;
using UnityEditor;

[CustomEditor(typeof(GameHandling))]
[CanEditMultipleObjects]
public class GameHandlingEditor : Editor
{
    private GUILayoutOption buttonOption = GUILayout.Width(300);

    public override void OnInspectorGUI()
    {
        DrawDefaultInspector();
        GameHandling gs = (GameHandling)target;
        GUILayout.Space(20);
        if (GUILayout.Button("Restart Game",buttonOption))
        {
            gs.restartGame();
        }
//
//        GUILayout.Space(20);
//        EditorGUILayout.LabelField(
//            akkaSystem.isSystemInstanciated() ? "System Instantiated" : "System NOT Instantiated");
//        GUILayout.Space(20);
//        EditorGUILayout.LabelField(akkaSystem.getSystemInfo(),GUILayout.Height(100));
//        GUILayout.Space(50);
//
//        if (GUILayout.Button("Get actors defined",buttonOption))
//        {
//            var children = akkaSystem.getSystemChlidren();
//            if (children.Count == 0)
//            {
//                EditorUtility.DisplayDialog("List Of Actors", "No Actors defined yet", "OK");
//            }
//            else
//            {
//                EditorUtility.DisplayDialog("List Of Actors", children.Aggregate((a, b) => $"{a}\n{b}"), "OK");
//            }
//            
//        }
    }
}
#endif