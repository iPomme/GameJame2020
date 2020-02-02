using System;
using System.Collections;
using UnityEngine;

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

        public void restartGame()
        {
            foreach (GameObject hole in holes)
            {
            }
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