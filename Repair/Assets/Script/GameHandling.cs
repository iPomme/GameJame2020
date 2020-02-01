using System;
using System.Collections;
using System.Collections.Generic;
using DefaultNamespace;
using UnityEngine;

public class GameHandling : MonoBehaviour
{
    public GameStatus gs;

    public GameObject invertedSphere;

    public GameObject waterLevel;

    private Renderer invertedSphereRenderer;

    private Coroutine _fadeCoroutine;
    private Coroutine _gameoverCoroutine;
    private Color _originalColor;

    void Start()
    {
        invertedSphereRenderer = invertedSphere.GetComponentInChildren<Renderer>();
        hideInvertedSphere();
        _originalColor = invertedSphereRenderer.material.color;
    }

    // Update is called once per frame
    void Update()
    {
        if (gs.gameover)
        {
            Debug.Log("GameOver");
            if (_gameoverCoroutine == null) _gameoverCoroutine = StartCoroutine(blackInvertedSphere());
            return;
        }
        else
        {
            _gameoverCoroutine = null;
        }

        if (gs.headsetLevel < gs.waterLevel)
        {
            if (_fadeCoroutine == null)
                _fadeCoroutine = StartCoroutine(Fade());
        }
        else
        {
            if (_fadeCoroutine != null)
            {
                StopCoroutine(_fadeCoroutine);
                _fadeCoroutine = null;
            }

            hideInvertedSphere();
        }
    }

    IEnumerator Fade()
    {
        for (float ft = 0f; ft <= 1; ft += 0.1f)
        {
            Color c = invertedSphereRenderer.material.color;
            c.a = ft;
            invertedSphereRenderer.material.color = c;
            Debug.Log(ft);
            if (ft >= .9f)
            {
                gs.gameover = true;
            }
            yield return new WaitForSeconds(.1f);
        }
    }

    private void hideInvertedSphere()
    {
        invertedSphereRenderer.material.color = new Color(invertedSphereRenderer.material.color.r,
            invertedSphereRenderer.material.color.g, invertedSphereRenderer.material.color.b, 0f);
    }

    private IEnumerator blackInvertedSphere()
    {
        yield return new WaitForSeconds(2f);
        invertedSphereRenderer.material.color = new Color(0, 0, 0, 1f);
        invertedSphereRenderer.material.DisableKeyword("_EMISSION");
    }

    private void OnDestroy()
    {
        gs.gameover = false;
    }

    public void restartGame()
    {
        waterLevel.transform.position = Vector3.zero;
        invertedSphereRenderer.material.color = _originalColor;
        hideInvertedSphere();
        gs.gameover = false;
    }
}