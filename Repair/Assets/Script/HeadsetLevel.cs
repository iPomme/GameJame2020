using System.Collections;
using System.Collections.Generic;
using DefaultNamespace;
using UnityEngine;

public class HeadsetLevel : MonoBehaviour
{

    
    public GameStatus gs;
    // Start is called before the first frame update
    void Start()
    {
        
    }

    // Update is called once per frame
    void Update()
    {
        gs.headsetLevel = transform.position.y;
    }
}
