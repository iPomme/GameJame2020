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
    }
}
#endif