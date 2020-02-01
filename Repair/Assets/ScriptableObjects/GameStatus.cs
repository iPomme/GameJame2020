using System;
using System.Collections;
using UnityEngine;

namespace DefaultNamespace
{
    [UnityEngine.CreateAssetMenu(fileName = "GameStatus", menuName = "Repair/GameStatus", order = 0)]
    public class GameStatus : UnityEngine.ScriptableObject
    {
        [Header("Global configuration")] 
        public int waterLevelSpeed;
        public int waterLevelCheckIntervalInSeconds;

        public int FailureGeneratorIntervalInSecond;
        public int NbOfHoles
        {
            get => holes.Length;
            set { holes = new Hole[value]; }
        }

        [Header("Game Status")] public float headsetLevel;
        public float waterLevel;
        public bool gameover;

        private Hole[] holes;
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
        ONE,
        TWO,
        THRE
    }
}