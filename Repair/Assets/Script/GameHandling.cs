using System.Collections;
using System.Linq;
using DefaultNamespace;
using UnityEngine;
using Random = UnityEngine.Random;

public class GameHandling : MonoBehaviour
{
    public GameStatus gs;

    public GameObject invertedSphere;

    public GameObject waterLevel;

    public GameObject[] goupilles;

    private Renderer _invertedSphereRenderer;

    private Color _originalColor;

    void Start()
    {
        _invertedSphereRenderer = invertedSphere.GetComponentInChildren<Renderer>();
        // Get the initial color of the inversed sphere.
        HideInvertedSphere();
        _originalColor = _invertedSphereRenderer.material.color;

        RestartGame();
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
                _gameoverCoroutine = StartCoroutine(GameOverControl());
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

            HideInvertedSphere();
        }
    }


    /**
     * This is gameover, close everything and stop everything
     */
    private void ApplyGameOver()
    {
        Debug.Log("GameOver");
        gs.gameover = true;
        if (_waterLevelCoroutine != null) StopCoroutine(_waterLevelCoroutine);
        if (_failureGeneratorCoroutine != null) StopCoroutine(_failureGeneratorCoroutine);
        _invertedSphereRenderer.material.color = new Color(0, 0, 0, 1f);
        _invertedSphereRenderer.material.DisableKeyword("_EMISSION");
    }

    /**
     * A new game is starting setup everything and let's play...
     */
    public void RestartGame()
    {
        waterLevel.transform.position = Vector3.zero;
        _invertedSphereRenderer.material.color = _originalColor;
        HideInvertedSphere();
        _waterLevelCoroutine = StartCoroutine(WaterLevelControl());
        _failureGeneratorCoroutine = StartCoroutine(FailureGeneratorControl());
        _initialAverageCoroutine = StartCoroutine(InitalAvarage());
        if (_gameoverCoroutine != null) StopCoroutine(_gameoverCoroutine);
        _gameoverCoroutine = null;
        gs.gameover = false;
    }

    private void HideInvertedSphere()
    {
        var material = _invertedSphereRenderer.material;
        material.color = new Color(material.color.r,
            material.color.g, material.color.b, 0f);
        _invertedSphereRenderer.enabled = false;
    }

    private void crash()
    {
        Debug.Log("Crash!!...");
    }

    private void FixAllHoles()
    {
        Debug.Log("FixHoles...");
        foreach (GameObject ecoutille in goupilles)
        {
            ecoutille.GetComponent<Ecoutille>().fixIt();
        }
    }

    private void GenerateAverage()
    {
        Debug.Log("Generate Average...");
        foreach (GameObject ecoutille in goupilles)
        {
            if (Random.Range(0f, 1f) > .7f)
            {
                ecoutille.GetComponent<Ecoutille>().brokeIt();
            }    
        }
    }
    
    /*
     * Get the number of broken holes
     */
    private int NumberOfBrokenHole()
    {
        return goupilles.Where(x => x.GetComponent<Ecoutille>().isBroken()).Count();
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
    private Coroutine _initialAverageCoroutine;


    /**
     * Generate the initial average
     */
    private IEnumerator InitalAvarage()
    {
        yield return new WaitForSeconds(4f);
        crash();
        FixAllHoles();
        GenerateAverage();
    }

    /**
     * Control the level of the water depending the number of active holes
    */
    private IEnumerator WaterLevelControl()
    {
        for (;;)
        {
            yield return new WaitForSeconds(gs.waterLevelCheckIntervalInSeconds);
            Debug.LogFormat("WATER LEVEL: {0}", gs.waterLevel);
            var transformPosition = waterLevel.transform.position;
            
            transformPosition.y = transformPosition.y + gs.waterLevelSpeed * NumberOfBrokenHole() * .001f;
            waterLevel.transform.position = transformPosition;
        }
    }

    /**
     * Create failure randomly
     */
    private IEnumerator FailureGeneratorControl()
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
    private IEnumerator GameOverControl()
    {
        for (int i = 0; i < gs.underWaterSecondBeforeGameOver * 10; i++)
        {
            Debug.LogFormat("GameOver: iter({0})", i);
            yield return new WaitForSeconds(.1f);
        }

        Debug.Log("APPLY GAMEOVER");
        ApplyGameOver();
    }

    IEnumerator Fade()
    {
        _invertedSphereRenderer.enabled = true;

        for (float ft = 0f; ft <= 1; ft += 0.1f)
        {
            var material = _invertedSphereRenderer.material;
            Color c = material.color;
            c.a = ft;
            material.color = c;
            yield return new WaitForSeconds(.1f);
        }
    }

    #endregion
}