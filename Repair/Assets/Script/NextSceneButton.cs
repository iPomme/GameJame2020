using System;
using System.Collections;
using System.Collections.Generic;
using DefaultNamespace;
using UnityEngine;

public class NextSceneButton : MonoBehaviour
{
    public GameStatus gh;


    private bool nextSceneAsked;

    private void OnEnable()
    {
        nextSceneAsked = false;
    }

    private void OnTriggerEnter(Collider other)
    {
        Debug.LogFormat("collision with {0}", other.gameObject.name);
        if (other.gameObject.name == "StartButton" && !nextSceneAsked)
            gh.getNextScene();
    }
}