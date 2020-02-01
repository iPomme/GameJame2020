using System.Collections;
using System.Collections.Generic;
using DefaultNamespace;
using UnityEngine;

public class WaterLevel : MonoBehaviour
{

    
    public GameStatus gs;
    // Start is called before the first frame update
    void Start()
    {
        
    }

    // Update is called once per frame
    void Update()
    {
        gs.waterLevel = transform.position.y;
        gs.headsetLevel = OVRManager.tracker.GetPose(0).position.y;
    }
}
