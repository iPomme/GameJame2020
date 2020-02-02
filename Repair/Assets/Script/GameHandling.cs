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

    private Color _originalColor;

    void Start()
    {
        invertedSphereRenderer = invertedSphere.GetComponentInChildren<Renderer>();
        hideInvertedSphere();
        _originalColor = invertedSphereRenderer.material.color;
        Debug.Log("GameHandling Started.");
    }

    // Update is called once per frame
    void Update()
    {
        if (gs.headsetLevel < gs.waterLevel)
        {
            if (_fadeCoroutine == null)
                _fadeCoroutine = StartCoroutine(Fade());
            if (_gameoverCoroutine == null) 
                _gameoverCoroutine = StartCoroutine(gameOverControl());
        }
        else
        {
            if (_fadeCoroutine != null)
            {
                StopCoroutine(_fadeCoroutine);
                _fadeCoroutine = null;
            }

            if (_gameoverCoroutine != null)
            {
                StopCoroutine(_gameoverCoroutine);
                _gameoverCoroutine = null;
            }

            hideInvertedSphere();
        }
    }


    /**
     * This is gameover, close everything and stop everything
     */
    private void applyGameOver()
    {
        Debug.Log("GameOver");
        gs.gameover = true;
        if (_waterLevelCoroutine != null) StopCoroutine(_waterLevelCoroutine);
        if (_failureGeneratorCoroutine != null) StopCoroutine(_failureGeneratorCoroutine);
        invertedSphereRenderer.material.color = new Color(0, 0, 0, 1f);
        invertedSphereRenderer.material.DisableKeyword("_EMISSION");
    }

    /**
     * A new game is starting setup everything and let's play...
     */
    public void restartGame()
    {
        waterLevel.transform.position = Vector3.zero;
        invertedSphereRenderer.material.color = _originalColor;
        hideInvertedSphere();
        _waterLevelCoroutine = StartCoroutine(waterLevelControl());
        _failureGeneratorCoroutine = StartCoroutine(failureGeneratorControl());
        if(_gameoverCoroutine != null) StopCoroutine(_gameoverCoroutine);
        _gameoverCoroutine = null;
        gs.gameover = false;
    }

    private void hideInvertedSphere()
    {
        invertedSphereRenderer.material.color = new Color(invertedSphereRenderer.material.color.r,
            invertedSphereRenderer.material.color.g, invertedSphereRenderer.material.color.b, 0f);
        invertedSphereRenderer.enabled = false;
    }

    /**
     * Clean up the GameStatus Singleton resource
     */
    private void OnDestroy()
    {
        gs.gameover = false;
    }

    #region Timers

    private Coroutine _fadeCoroutine;
    private Coroutine _gameoverCoroutine;
    private Coroutine _waterLevelCoroutine;
    private Coroutine _failureGeneratorCoroutine;

    /**
     * Control the level of the water depending the number of active holes
    */
    private IEnumerator waterLevelControl()
    {
        for (;;)
        {
            yield return new WaitForSeconds(gs.waterLevelCheckIntervalInSeconds);
            Debug.LogFormat("WATER LEVEL: {0}", gs.waterLevel);
        }
    }

    /**
     * Create failure randomly
     */
    private IEnumerator failureGeneratorControl()
    {
        for (;;)
        {
            yield return new WaitForSeconds(gs.FailureGeneratorIntervalInSecond);
            Debug.LogFormat("Let's Break some Holes....");
        }
    }

    /**
     * Control the time allowed underwater, once reached, it's gameover, that's life!
     */
    private IEnumerator gameOverControl()
    {
        for (int i = 0; i < gs.underWaterSecondBeforeGameOver * 10; i++)
        {
            Debug.LogFormat("GameOver: iter({0})",i);
            yield return new WaitForSeconds(.1f);
        }
        Debug.Log("APPLY GAMEOVER");
        applyGameOver();
    }

    IEnumerator Fade()
    {
        invertedSphereRenderer.enabled = true;

        for (float ft = 0f; ft <= 1; ft += 0.1f)
        {
            Color c = invertedSphereRenderer.material.color;
            c.a = ft;
            invertedSphereRenderer.material.color = c;
            yield return new WaitForSeconds(.1f);
        }
    }

    #endregion
}