using System;
using System.Collections;
using UnityEngine;
using UnityEngine.SceneManagement;

namespace DefaultNamespace
{
    [UnityEngine.CreateAssetMenu(fileName = "GameStatus", menuName = "Repair/GameStatus", order = 0)]
    public class GameStatus : UnityEngine.ScriptableObject
    {
        [Header("Global configuration")] public int waterLevelSpeed;
        public int waterLevelCheckIntervalInSeconds;
        public int underWaterSecondBeforeGameOver;

        public int FailureGeneratorIntervalInSecond;

        public int numberOfHulaToSpawn;

        [Header("Game Status")] public float headsetLevel;
        public float waterLevel;
        public bool gameover;

        public GameObject[] holes;

        public int currentScene;


        public int getNextScene()
        {
            UnityEngine.Debug.LogFormat("current scene: {0}, max scene: {1}", currentScene,
                SceneManager.sceneCountInBuildSettings);
            SceneManager.UnloadSceneAsync(currentScene);
            if (SceneManager.sceneCountInBuildSettings - 1 == currentScene)
            {
                SceneManager.LoadScene(0, LoadSceneMode.Single);
                currentScene = 0;
            }
            else
            {
                currentScene += 1;
                SceneManager.LoadSceneAsync(currentScene, LoadSceneMode.Single);
            }

            return currentScene;
        }
    }

    public struct Hole
    {
        public HoleStatus Status;
        public HoleShape Shape;

        public Hole(HoleStatus holeStatus, HoleShape holeShape)
        {
            this.Status = holeStatus;
            this.Shape = holeShape;
        }
    }

    public enum HoleStatus
    {
        FIXED,
        BROKEN
    }

    public enum HoleShape
    {
        ONE = 0,
        TWO = 1,
        THRE = 2
    }
}