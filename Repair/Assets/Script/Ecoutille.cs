using System.Collections;
using System.Collections.Generic;
using DefaultNamespace;
using UnityEngine;

public class Ecoutille : MonoBehaviour
{
    private Hole hole;
    private Animator _animator;
    private ParticleSystem _particle;


    public HoleShape getShape()
    {
        return hole.Shape;
    }

    public HoleStatus getStatus()
    {
        return hole.Status;
    }

    public void brokeIt()
    {
        hole.Status = HoleStatus.BROKEN;
        _animator.SetBool("isLeaking", true);
        var particleEmission = _particle.emission;
        particleEmission.enabled = true;
    }

    public void fixIt()
    {
        hole.Status = HoleStatus.FIXED;
        _animator.SetBool("isLeaking", false);
        var particleEmission = _particle.emission;
        particleEmission.enabled = false;
    }

    // Start is called before the first frame update
    void Start()
    {
        hole = new Hole(HoleStatus.FIXED, (HoleShape) Random.Range(0, 2));
        _animator = GetComponentInChildren<Animator>();
        _particle = GetComponentInChildren<ParticleSystem>();
    }

    // Update is called once per frame
    void Update()
    {
    }
}